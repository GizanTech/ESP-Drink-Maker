WIFI-CONTROLLED STEPPER MOTORS WITH ESP8266

Low cost open hardware driver board for controlling two stepper motors
over WiFi, and needs no Arduino/Raspberry Pi/etc

Full details, photos & video at:
https://climbers.net/sbc/esp-wifi-stepper-motors-controller-revb/

Nikki Smith, March 2020.

Files:
-----

data/                   Flash filesystem: Web pages, etc
Adafruit_MCP23017.cpp   MCP23017 library for I/O expander chip
Adafruit_MCP23017.h
ESPmotorsB.bin          Compiled firmware
ESPmotorsB.spiffs       Compiled flash filesystem
ESPmotorsB.h            User settings, pin names
ESPmotorsB.ino          Main source code
Firmware.ino            Firmware update (OTA) source code
WebServer.ino           Webserver source code
espmotors.yaml          Config file for using with ESPHome.io project

Changelog:
---------

Mar/2022
- Release v1.2
- Compiled with esp8266 core v3.0.2
- Floating point motor run times: 5.0 sec, 0.2 sec, etc
- WiFi transmit power increase from minimum to 50%
- Added espmotors.yaml for using with ESPHome.io project
- Added support for uploading OTA firmware direct from Arduino IDE
  added support for uploading new SPIFFS via web server
  Split off firmware update functions into new Firmware.ino
- New ESPMOTORS_STATION compile-time option in ESPmotorsB.h for joining 
  your existing WiFi network, new config file data/wifi-station.txt
- Renamed WiFi AP password file from data/pwd.txt to data/wifi-ap.txt

Mar/2020
- Initial release v1.0
