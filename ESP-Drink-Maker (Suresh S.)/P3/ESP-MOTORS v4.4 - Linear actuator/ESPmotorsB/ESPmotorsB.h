/*
   ESPmotorsB.h
   User settings for ESPmotors revB sketch
   https://Climbers.net (c) 2018-2022
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

#define number_of_bottles 21
#define number_of_drinks 20
#define oneStepDistance 0.2 //We need to calibrate this one.

long bottle_distance[number_of_bottles + 1];
long bottle_height[number_of_bottles + 1];

// This defines the number of drinks it will mix
long drink_combination[number_of_drinks + 1][20] = {
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // ignore this drink
  { 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},      // drink-1 combination with bottle number
  { 1, 2, 1, 2, 2, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},         // drink-2 combination with bottle number
  { 1, 18, 2, 17, 10, 3, 18, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // drink-3 combination with bottle number
  { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, -1, -1}, // drink-4 combination
  { 1, 20, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

// This defines the time it will hold dispenser on each bottle.(in seconds)
long drink_hold_time[number_of_drinks + 1][20] = {
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // ignore this drink
  { 4, 4, 4, 4, 4, 8, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},       // drink-1 Stirring has 4 seconds and dispense has 10 seconds
  { 4, 4, 4, 4, 4, 4, -1, -1, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, -1},         // drink-2 combination with bottle holding time
  { 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // drink-3 combination with bottle holding time
  { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, -1, -1}, // drink-4 combination
  { 1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

void declare_variables() {
  long bottle_num;

  bottle_num = 1;
  bottle_distance[bottle_num] = -1888; //0 mm
  bottle_height[bottle_num] = -169; //mm ISSUES

  bottle_num = 2;
  bottle_distance[bottle_num] = -1790 ; //mm
  bottle_height[bottle_num] = -164; //mm

  bottle_num = 3;
  bottle_distance[bottle_num] = -1691; //mm
  bottle_height[bottle_num] = -164; //mm

  bottle_num = 4;
  bottle_distance[bottle_num] = -1588; //   300 mm
  bottle_height[bottle_num] = -163; //mm  ISSUES

  bottle_num = 5;
  bottle_distance[bottle_num] = -1495; //mm
  bottle_height[bottle_num] = -162; //mm ISSUES

  bottle_num = 6;
  bottle_distance[bottle_num] = -1392; //mm
  bottle_height[bottle_num] = -157; //mm

  bottle_num = 7;
  bottle_distance[bottle_num] = -1294; //mm
  bottle_height[bottle_num] = -155; //mm ISSUES

  bottle_num = 8;
  bottle_distance[bottle_num] = -1190; //mm
  bottle_height[bottle_num] = -156; //mm

  bottle_num = 9;
  bottle_distance[bottle_num] = -1095; // 793 mm
  bottle_height[bottle_num] = -156; //mm

  bottle_num = 10;
  bottle_distance[bottle_num] = -995; //893 mm
  bottle_height[bottle_num] = -156; //mm

  bottle_num = 11;
  bottle_distance[bottle_num] = -896; // 992 mm
  bottle_height[bottle_num] = -157; //mm

  bottle_num = 12;
  bottle_distance[bottle_num] = -793; // 1095 mm
  bottle_height[bottle_num] = -158; //mm

  bottle_num = 13;
  bottle_distance[bottle_num] = -699; // 1189 mm
  bottle_height[bottle_num] = -136; //mm

  bottle_num = 14;
  bottle_distance[bottle_num] = -599; // 1289 mm
  bottle_height[bottle_num] = -137; //mm

  bottle_num = 15;
  bottle_distance[bottle_num] = -502; //mm
  bottle_height[bottle_num] = -135; //mm

  bottle_num = 16;
  bottle_distance[bottle_num] = -400; //1488 mm
  bottle_height[bottle_num] = -136; //mm

  bottle_num = 17;
  bottle_distance[bottle_num] = -297; //mm
  bottle_height[bottle_num] = -136; //mm

  bottle_num = 18;
  bottle_distance[bottle_num] = -201; //1687 mm
  bottle_height[bottle_num] = -137; //mm

  bottle_num = 19;
  bottle_distance[bottle_num] = 1787; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 20;
  bottle_distance[bottle_num] = 0; //1888 mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 21;
  bottle_distance[bottle_num] = 0; //1888 mm
  bottle_height[bottle_num] = +50; //mm

  for (int i = 1; i <= 21 ; i++) {
    Serial.print("Bottle\tDist\tHeight\n");
    Serial.print(i); Serial.print("\t");
    Serial.print(bottle_distance[i]); Serial.print("\t");
    Serial.print(bottle_height[i]); Serial.print("\n");

  }
}

#endif
