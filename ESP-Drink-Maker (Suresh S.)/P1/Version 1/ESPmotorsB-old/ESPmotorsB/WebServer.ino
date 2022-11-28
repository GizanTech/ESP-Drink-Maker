/* 
 * WebServer.ino
 * Web server & DNS server functions for ESPmotors device
 * https://Climbers.net (c) 2018-2022
 */

#include <ESP8266WiFi.h>
extern "C" {
#include <stdlib.h>
}
#include "FS.h"
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>

ESP8266WebServer server(80);

#ifndef ESPMOTORS_STATION
#include <DNSServer.h>
DNSServer dnsServer;
const byte DNS_PORT = 53;
#endif


// Redirect to captive portal if we got a request for another domain
// Return true in that case so the page handler does not try to handle the request again
// we can't use "<something>.local" as iOS/Linux special case that domain to use mDNS (?)
bool captivePortal() {
#ifndef ESPMOTORS_STATION
  if (server.hostHeader() != String("espmotors.home") && server.hostHeader() != WiFi.softAPIP().toString().c_str()) {
    Serial.printf("captivePortal(): Request for '%s'\r\n", server.hostHeader().c_str());
    server.sendHeader("Location", String("http://espmotors.home"), true);
    
    // Empty content inhibits Content-length header so we have to close the socket ourselves
    server.send(302, "text/plain", ""); 
    server.client().stop();
    return true;
  }
#endif
  return false;
}


// Process FORM arguments from main motors web page
void html_motors() {
  if (captivePortal()) return;
  bool motor1 = (server.arg("motor1") == "1");
  int speed1 = server.arg("speed1").toInt();
  int step1 = server.arg("step1").toInt();
  bool forward1 = (server.arg("dirn1") != "1");
  bool limit1 = (server.arg("limit1") == "1");
  bool motor2 = (server.arg("motor2") == "1");
  int speed2 = server.arg("speed2").toInt();
  int step2 = server.arg("step2").toInt();
  bool forward2 = (server.arg("dirn2") != "1");
  bool limit2 = (server.arg("limit2") == "1");
  float secs = server.arg("time").toFloat();
  run_motors(motor1,speed1,step1,forward1,limit1, motor2,speed2,step2,forward2,limit2, secs);
  server.send(200, "text/plain", "OK");
  Serial.println("html_motors(): finished");
}


// Connect to WiFi, setup internal web server, DNS & OTA
void server_setup() {
  Serial.begin(115200);
  Serial.println();
  // Open up local file system
  if (!SPIFFS.begin()) {
    Serial.println("server_setup(): Flash FS failed");
  }
  // Read WLAN password from SPIFFS file? and ssid if ESPMOTORS_STATION
  File file = SPIFFS.open(WLAN_CONFIG, "r");
  String wlan_ssid, wlan_pwd;
  if (file) {
#ifdef ESPMOTORS_STATION
    wlan_ssid = file.readStringUntil('\n');
    wlan_ssid.trim(); // trim any '\r' char from end
    wlan_pwd = file.readStringUntil('\n'); // must be 8..63 chars long
    wlan_pwd.trim(); // trim any '\r' char from end
    //Serial.printf("server_setup(): wlan ssid '%s' pwd '%s'\r\n", wlan_ssid.c_str(), wlan_pwd.c_str());
#else
    wlan_ssid = WLAN_SSID + String(ESP.getChipId(),HEX);
    wlan_pwd = file.readStringUntil('\n'); // must be 8..63 chars long
    wlan_pwd.trim(); // trim any '\r' char from end
    //Serial.printf("server_setup(): wlan pwd '%s'\r\n", wlan_pwd.c_str());
#endif
    file.close();
  } else {
    Serial.printf("server_setup(): Missing config file '%s'\r\n", WLAN_CONFIG);
    wlan_pwd = ""; // empty string means open WiFi, no password required
    wlan_ssid = "";
  }
  
  // Power reduction for increased battery life by: 
  // Limit range of WiFi (+increase security): 0=minimum, 20.5=maximum
  // Use 802.11n mode
  WiFi.setOutputPower(10);
  WiFi.setPhyMode(WIFI_PHY_MODE_11N);
  
#ifdef ESPMOTORS_STATION
  // Connect to local WiFi as a station
  WiFi.mode(WIFI_STA);
  Serial.printf("server_setup(): WiFi Station. Connecting to '%s'...\r\n", wlan_ssid.c_str());
  WiFi.begin(wlan_ssid, wlan_pwd);
  uint8_t status = WiFi.waitForConnectResult();
  if (status != WL_CONNECTED) {
    Serial.printf("server_setup(): WiFi Station. Failed to connect, status %d. Restarting in 30 sec...\r\n", status);
    delay(30*1000);
    ESP.restart();
  } else {
    Serial.printf("server_setup(): WiFi Station. Connected, signal strength %d dBm\r\n", WiFi.RSSI());
  }
#else
  // Start WiFi Access Point
  WiFi.softAP(wlan_ssid.c_str(), wlan_pwd.c_str(), 1);
  Serial.printf("server_setup(): WiFi Access Point '%s' started\r\n", wlan_ssid.c_str());

  // Setup Captive Portal (intercept DNS requests from clients)
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  Serial.printf("server_setup(): DNS server started on '%s'\r\n", WiFi.softAPIP().toString().c_str());
#endif

  // Setup our web server to serve pages, optional pre-compressed .gz version
  server.serveStatic("/gears.png", SPIFFS, "/gears.png", "max-age=86400");
  server.on("/motors", HTTP_GET, html_motors);
  server.on("/", HTTP_GET, []() {
    if (captivePortal()) return;
    // Does web client accept gzip content?
    bool gzip = (strstr(server.header("Accept-Encoding").c_str(), "gzip") != NULL);
    File f = SPIFFS.open(gzip ? WEB_HOMEPAGE+String(".gz") : WEB_HOMEPAGE, "r");
    if (f) {
      // Adds Content-Length, Content-Type and (optional) Content-Encoding headers
      server.sendHeader("Cache-Control", "max-age=86400");
      server.streamFile(f, "text/html");
      f.close();
    } else {
      server.send(404, "text/plain", "Not Found");
    }
  });
  server.on("/update", HTTP_POST, [](){
    if (Update.hasError()) {
      server.send(200, "text/plain", "FAIL");
    } else {
      server.sendHeader("Location", "/?updated=" + String(random(10000)), true);
      server.send(303, "text/plain", "See Other");
    }
    server.client().stop();
    ESP.restart();
  }, firmware_updateweb);  
  server.onNotFound([](){
    if (captivePortal()) return;
    server.send(404, "text/plain", "Not Found");
  });
  // We'll need this HTTP header later
  const char *headerkeys[] = {"Accept-Encoding"};
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  server.collectHeaders(headerkeys, headerkeyssize);
  server.begin();
  firmware_updateota();
  Serial.print("server_setup(): HTTP server started at http://");
#ifdef ESPMOTORS_STATION
  Serial.println(WiFi.localIP());
#else
  Serial.println(WiFi.softAPIP());
#endif
}


// the loop function runs over and over again forever
void server_loop() {
#ifndef ESPMOTORS_STATION
  dnsServer.processNextRequest();
#endif
  server.handleClient();
  ArduinoOTA.handle();
}
