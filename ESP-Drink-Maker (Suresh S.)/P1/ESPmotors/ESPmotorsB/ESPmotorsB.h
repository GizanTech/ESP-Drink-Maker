/* 
 * ESPmotorsB.h
 * User settings for ESPmotors revB sketch
 * https://Climbers.net (c) 2018-2022
 */

#ifndef ESPMOTORS_H
#define ESPMOTORS_H

// Uncomment to join your existing WiFi network
#define ESPMOTORS_STATION

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

// ESP GPIOs (GPIO13 and A0 pins are spare)
// use pinMode() / digitalRead() / digitalWrite()
#define STEP1       14
#define STEP2       12

// MCP GPIOs
// use mcp.pinMode() / mcp.digitalRead() / mcp.digitalWrite()
#define DIRN1       0   // GPA0
#define MOTOR1      1   // GPA1
#define SPEED1_MS3  2   // GPA2
#define SPEED1_MS2  3   // GPA3
#define SPEED1_MS1  4   // GPA4
#define GPA5        5   // spare
#define GPA6        6   // spare
#define LIMIT1      7   // GPA7
#define DIRN2       8   // GPB0
#define MOTOR2      9   // GPB1
#define SPEED2_MS3  10  // GPB2
#define SPEED2_MS2  11  // GPB3
#define SPEED2_MS1  12  // GPB4
#define GPB5        13  // spare
#define GPB6        14  // spare
#define LIMIT2      15  // GPB7

#endif
