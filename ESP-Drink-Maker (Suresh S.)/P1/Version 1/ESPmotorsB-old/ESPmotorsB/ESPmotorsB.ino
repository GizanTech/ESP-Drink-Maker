/* 
 * ESPmotorsB.ino
 * Default firmware v1.2 for ESP8266 stepper motors controller revB
 * 
 * Board: Generic ESP8266 Module (esp8266 core 3.0.2)
 * CPU Frequency: 80 MHz
 * Upload Speed: 460800
 * Flash Size: 1M-4M (64K FS minimum)
 * 
 * https://Climbers.net (c) 2018-2022
 */

// Adafruit MCP23017 I/O expander library
// https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library
#include "Adafruit_MCP23017.h"
#include "ESPmotorsB.h"

Adafruit_MCP23017 mcp;


// Run the motors using parameters from the web form
void run_motors(bool motor1, int speed1, int step1, bool forward1, bool limit1, 
                bool motor2, int speed2, int step2, bool forward2, bool limit2,
                float secs) {  
  // Setup motor #1
  if (motor1) {
    Serial.printf("run_motors(): motor1=%d %d %d %d\r\n", speed1, step1, forward1, limit1);
    pinMode(STEP1, OUTPUT);
    digitalWrite(STEP1, LOW);
    mcp.pinMode(DIRN1, OUTPUT);
    mcp.pinMode(SPEED1_MS3, OUTPUT);
    mcp.pinMode(SPEED1_MS2, OUTPUT);
    mcp.pinMode(SPEED1_MS1, OUTPUT);
    mcp.pinMode(MOTOR1, OUTPUT);
    mcp.digitalWrite(DIRN1, forward1 ? LOW : HIGH); // Pull DIRN pin LOW to move "forward"
    step1 = step1 < 5 ? step1-1 : 7; // Microstepping mode 1-5 convert to bitfield 0-7
    mcp.digitalWrite(SPEED1_MS1, step1 & 1 ? HIGH : LOW);
    mcp.digitalWrite(SPEED1_MS2, step1 & 2 ? HIGH : LOW);
    mcp.digitalWrite(SPEED1_MS3, step1 & 4 ? HIGH : LOW);
    mcp.digitalWrite(MOTOR1, HIGH); // Enable motor, ready for stepping
  };
  
  // Setup motor #2
  if (motor2) {
    Serial.printf("run_motors(): motor2=%d %d %d %d\r\n", speed2, step2, forward2, limit2);
    pinMode(STEP2, OUTPUT);
    digitalWrite(STEP2, LOW);
    mcp.pinMode(DIRN2, OUTPUT);
    mcp.pinMode(SPEED2_MS3, OUTPUT);
    mcp.pinMode(SPEED2_MS2, OUTPUT);
    mcp.pinMode(SPEED2_MS1, OUTPUT);
    mcp.pinMode(MOTOR2, OUTPUT);
    mcp.digitalWrite(DIRN2, forward2 ? LOW : HIGH); // Pull DIRN pin LOW to move "forward"
    step2 = step2 < 5 ? step2 -1 : 7; // Microstepping mode 1-5 convert to bitfield 0-7
    mcp.digitalWrite(SPEED2_MS1, step2 & 1 ? HIGH : LOW);
    mcp.digitalWrite(SPEED2_MS2, step2 & 2 ? HIGH : LOW);
    mcp.digitalWrite(SPEED2_MS3, step2 & 4 ? HIGH : LOW);
    mcp.digitalWrite(MOTOR2, HIGH); // Enable motor, ready for stepping
  };
  
  // Run motors for 'secs'
  // timer subtraction works correctly even when micros() wraps (every 71 mins)
  unsigned long tstart, t1, t2, tyield, t;
  tstart = tyield = t1 = t2 = micros();
  while (micros() - tstart < secs*1000000.0) {
    t = micros();
    if (motor1 && t-t1 >= 1000000/speed1) {
      if (limit1 && mcp.digitalRead(LIMIT1) == LOW) {
        Serial.println("run_motors(): Limit switch pressed to stop motor #1!");
        motor1 = false;
        continue;
      };
      digitalWrite(STEP1, HIGH); // Trigger one step, at least 1us pulse width
      digitalWrite(STEP1, LOW);  // Pull step pin low so it can be triggered again
      t1 = t;
    };
    if (motor2 && t-t2 >= 1000000/speed2) {
      if (limit2 && mcp.digitalRead(LIMIT2) == LOW) {
        Serial.println("run_motors(): Limit switch pressed to stop motor #2!");
        motor2 = false;
        continue;
      };
      digitalWrite(STEP2, HIGH); // Trigger one step, at least 1us pulse width
      digitalWrite(STEP2, LOW);  // Pull step pin low so it can be triggered again
      t2 = t;
    };
    // Run 'housekeeping' tasks (e.g., WiFi) every 0.5 sec
    if (t - tyield >= 500000) {
      tyield = t;
      yield(); 
    };
    // Wait upto 0.0002 sec before next iteration (max speed=5000 steps/sec)
    if (micros() - t < 200) delayMicroseconds(200-(micros()-t));
  };
  
  // Disable any motors (saves power)
  mcp.pinMode(MOTOR1, INPUT);
  mcp.pinMode(MOTOR2, INPUT);
}


// the setup function runs once when you trigger reset or power the board
void setup() {
  server_setup();
  mcp.begin();
}


// the loop function runs over and over again forever
void loop() {
  server_loop();
}
