/* 
 * Firmware.ino
 * Firmware Over-The-Air updates for ESPmotors device
 * https://Climbers.net (c) 2018-2022
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <flash_hal.h>

extern ESP8266WebServer server;


// Flash new firmware uploaded by web form, see <Updater.h>
void firmware_updateweb() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.setDebugOutput(true);
    WiFiUDP::stopAll();
    Serial.printf("firmware_updateweb(): %s=%s\r\n", upload.name.c_str(), upload.filename.c_str());
    if (upload.name == "filesystem") {
      //FSInfo fs_info;
      //SPIFFS.info(fs_info);
      size_t fsSize = ((size_t) &_FS_end - (size_t) &_FS_start);
      close_all_fs();
      if (!Update.begin(fsSize, U_FS)){ // fs_info.totalBytes
        Update.printError(Serial);
      }
    } else {
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace, U_FLASH)) { // start with max available size
        Update.printError(Serial);
      }
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) { // true to set the size to the current progress
      Serial.printf("firmware_updateweb(): Success %u\r\nRebooting...\r\n", upload.totalSize);
    } else {
      Update.printError(Serial);
    }
    Serial.setDebugOutput(false);
  }
  yield();
}


// Setup Arduino OTA for flashing new firmware
void firmware_updateota() {
  // Port defaults to 8266
  //ArduinoOTA.setPort(8266);
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname((WLAN_SSID + String(ESP.getChipId(),HEX)).c_str());
  // No authentication by default
  //ArduinoOTA.setPassword((const char *)"SECRET-WORD");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
      SPIFFS.end(); // unmount FS
    }
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}
