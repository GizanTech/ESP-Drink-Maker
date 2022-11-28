/*
   ESPmotorsB.ino
   Board: Generic ESP8266 Module (esp8266 core 3.0.2)
   CPU Frequency: 80 MHz
   Upload Speed: 460800
   Flash Size: 1M-4M (64K FS minimum)
*/
#include "Adafruit_MCP23017.h"
#include "ESPmotorsB.h"

Adafruit_MCP23017 mcp;

#define motor1_speed 200 //steps/seconds (min: 200 & max: 5000)
#define motor2_speed 200 //steps/seconds (min: 200 & max: 5000)

/*
   Microstep mode -> Microstep
   Mode-1         -> full
   Mode-2         -> 1/2
   Mode-3         -> 1/4
   Mode-4         -> 1/8
   Mode-5         -> 1/16
*/
#define motor1_step_mode 1 //Just declare the mode
#define motor2_step_mode 1 //Just declare the mode

#define forward_rotate 0
#define reverse_rotate 1

#define motor1_limit 0 //Ignore limit switch. To use limit switch, make the value 1.
#define motor2_limit 0 //Ignore limit switch. To use limit switch, make the value 1.

#define number_of_bottles 21
#define number_of_drinks 20

int bottle_distance[number_of_bottles + 1];
int bottle_height[number_of_bottles + 1];

int drink_combination[number_of_drinks + 1][20]={
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // ignore this drink
                  { 1, 1, 1, 1, 1, 2, 20, 21, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},      // drink-1 combination with bottle number
                  { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 21},         // drink-2 combination with bottle number
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // drink-3 combination with bottle number
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
                  };
                  
int drink_hold_time[number_of_drinks + 1][20]={
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // ignore this drink
                  { 4, 4, 4, 4, 4, 8, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},       // drink-1 Stirring has 4 seconds and dispense has 10 seconds
                  { 2, 2, 2, 2, 2, 2, 2, 2, 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 10},         // drink-2 combination with bottle holding time
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // drink-3 combination with bottle holding time
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
                  };

int current_position = 0;

void declare_variables() {
  int bottle_num;

  bottle_num = 1;
  bottle_distance[bottle_num] = 0; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 2;
  bottle_distance[bottle_num] = 100; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 3;
  bottle_distance[bottle_num] = 200; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 4;
  bottle_distance[bottle_num] = 300; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 5;
  bottle_distance[bottle_num] = 395; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 6;
  bottle_distance[bottle_num] = 495; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 7;
  bottle_distance[bottle_num] = 595; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 8;
  bottle_distance[bottle_num] = 695; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 9;
  bottle_distance[bottle_num] = 790; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 10;
  bottle_distance[bottle_num] = 890; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 11;
  bottle_distance[bottle_num] = 990; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 12;
  bottle_distance[bottle_num] = 1090; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 13;
  bottle_distance[bottle_num] = 1185; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 14;
  bottle_distance[bottle_num] = 1285; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 15;
  bottle_distance[bottle_num] = 1385; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 16;
  bottle_distance[bottle_num] = 1485; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 17;
  bottle_distance[bottle_num] = 1585; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 18;
  bottle_distance[bottle_num] = 1685; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 19;
  bottle_distance[bottle_num] = 1780; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 20;
  bottle_distance[bottle_num] = 1880; //mm
  bottle_height[bottle_num] = 32; //mm

  bottle_num = 21;
  bottle_distance[bottle_num] = 1880; //mm
  bottle_height[bottle_num] = -15; //mm
}

void getDrink(int drink) {
  Serial.print("Preparing Drink no: "); Serial.println(drink);
  
  for (int i = 0 ; i < 20; i++) {
    if (drink_combination[drink][i] != -1){
      int destination = bottle_distance[drink_combination[drink][i]];
      Serial.print("Destination X: "); Serial.println(destination);
      int distance = destination - current_position;
      float steps_needed = abs(distance) / 1.6;
      float time_needed = steps_needed / motor1_speed;
      Serial.print("Time X: "); Serial.println(time_needed);
      bool diraction = (distance >= 0) ? forward_rotate:reverse_rotate;
      current_position = destination;
      
      run_motors(true, motor1_speed, motor1_step_mode, diraction, motor1_limit, false, 0, 0, 0, 0, time_needed);
      delay(500);


      destination = bottle_height[drink_combination[drink][i]];
      distance = destination;
      Serial.print("Destination Y: "); Serial.println(destination);
      steps_needed = abs(distance) / 1.6;
      time_needed = steps_needed / motor1_speed;
      Serial.print("Time Y: "); Serial.println(time_needed);
      diraction = (distance >= 0) ? forward_rotate:reverse_rotate;
      
      run_motors(false, 0, 0, 0, 0, true, motor2_speed, motor2_step_mode, diraction, motor2_limit, time_needed);
      uint32_t holdingTime = drink_hold_time[drink][i] * 1000;
      delay(holdingTime);
          
      run_motors(false, 0, 0, 0, 0, true, motor2_speed, motor2_step_mode, !diraction, motor2_limit, time_needed);
      delay(500);
    }
  }
}

void setup() {
  declare_variables();
  server_setup();
  mcp.begin();
}

void loop() {
  server_loop();
}


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
    step1 = step1 < 5 ? step1 - 1 : 7; // Microstepping mode 1-5 convert to bitfield 0-7
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
    step2 = step2 < 5 ? step2 - 1 : 7; // Microstepping mode 1-5 convert to bitfield 0-7
    mcp.digitalWrite(SPEED2_MS1, step2 & 1 ? HIGH : LOW);
    mcp.digitalWrite(SPEED2_MS2, step2 & 2 ? HIGH : LOW);
    mcp.digitalWrite(SPEED2_MS3, step2 & 4 ? HIGH : LOW);
    mcp.digitalWrite(MOTOR2, HIGH); // Enable motor, ready for stepping
  };

  // Run motors for 'secs'
  // timer subtraction works correctly even when micros() wraps (every 71 mins)
  unsigned long tstart, t1, t2, tyield, t;
  tstart = tyield = t1 = t2 = micros();
  while (micros() - tstart < secs * 1000000.0) {
    t = micros();
    if (motor1 && t - t1 >= 1000000 / speed1) {
      if (limit1 && mcp.digitalRead(LIMIT1) == LOW) {
        Serial.println("run_motors(): Limit switch pressed to stop motor #1!");
        motor1 = false;
        continue;
      };
      digitalWrite(STEP1, HIGH); // Trigger one step, at least 1us pulse width
      digitalWrite(STEP1, LOW);  // Pull step pin low so it can be triggered again
      t1 = t;
    };
    if (motor2 && t - t2 >= 1000000 / speed2) {
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
    if (micros() - t < 200) delayMicroseconds(200 - (micros() - t));
  };

  // Disable any motors (saves power)
  mcp.pinMode(MOTOR1, INPUT);
  mcp.pinMode(MOTOR2, INPUT);
}
