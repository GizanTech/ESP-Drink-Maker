/* 
 * ESPmotorsB.h
 * User settings for ESPmotors revB sketch
 * https://Climbers.net (c) 2018-2022
 */

#ifndef ESPMOTORS_H
#define ESPMOTORS_H

// Uncomment to join your existing WiFi network
//#define ESPMOTORS_STATION

// Appends hex of getChipId()
const char* WLAN_SSID = "ESPmotors-";

// SPIFFS filename of hard-wired password, or empty/missing for no password required
#ifdef ESPMOTORS_STATION
const char* WLAN_CONFIG = "/wifi-station.txt";
#else
const char* WLAN_CONFIG = "/wifi-ap.txt";
#endif

// SPIFFS filename of hard-wired web server homepage
const char* WEB_HOMEPAGE = "/indexB.html";

#endif
