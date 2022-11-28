#include "ESPmotorsB.h"

#define Motor1 0
#define Motor2 1

#define motor1EnPin 16
#define motor2EnPin 5

#define delayBetweenPulse 2000
#define delayBetweenPulse2 2000

#define motor1_speed 200 //steps/seconds (min: 200 & max: 5000)
#define motor2_speed 200 //steps/seconds (min: 200 & max: 5000)

#define forward_rotate 0
#define reverse_rotate 1

#define motor1_limit 0 //Ignore limit switch. To use limit switch, make the value 1.
#define motor2_limit 0 //Ignore limit switch. To use limit switch, make the value 1.

#define oneStepDistance 0.16 //We need to calibrate this one.

#define number_of_bottles 21
#define number_of_drinks 20

int bottle_distance[number_of_bottles + 1];
int bottle_height[number_of_bottles + 1];

//Pin defining
int motor1DirPin = 15;
int motor2DirPin = 4;

int motor1StepPin = 13;
int motor2StepPin = 0;

int drink_combination[number_of_drinks + 1][20] = {
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // ignore this drink
  { 1, 1, 1, 1, 1, 2, 20, 21, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},      // drink-1 combination with bottle number
  { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 21},         // drink-2 combination with bottle number
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // drink-3 combination with bottle number
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
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

int drink_hold_time[number_of_drinks + 1][20] = {
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // ignore this drink
  { 4, 4, 4, 4, 4, 8, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},       // drink-1 Stirring has 4 seconds and dispense has 10 seconds
  { 2, 2, 2, 2, 2, 2, 2, 2, 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 10},         // drink-2 combination with bottle holding time
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // drink-3 combination with bottle holding time
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
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
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

  if (drink == 51) {
    test();
    return;
  }

  for (int i = 0 ; i < 20; i++) {
    if (drink_combination[drink][i] != -1) {
      int destination = bottle_distance[drink_combination[drink][i]];
      Serial.print("Destination X: "); Serial.println(destination);
      int distance = destination - current_position;
      float steps_needed = abs(distance) / oneStepDistance;
      float time_needed = steps_needed / motor1_speed;
      Serial.print("Time X: "); Serial.println(time_needed);
      bool diraction = (distance >= 0) ? forward_rotate : reverse_rotate;
      current_position = destination;
      
      runMotor(Motor1, steps_needed, diraction);
      delay(500);


      destination = bottle_height[drink_combination[drink][i]];
      distance = destination;
      Serial.print("Destination Y: "); Serial.println(destination);
      steps_needed = abs(distance) / oneStepDistance;
      time_needed = steps_needed / motor1_speed;
      Serial.print("Time Y: "); Serial.println(time_needed);
      diraction = (distance >= 0) ? forward_rotate : reverse_rotate;

      runMotor(Motor2, steps_needed, diraction);
      uint32_t holdingTime = drink_hold_time[drink][i] * 1000;
      delay(holdingTime);

      runMotor(Motor2, steps_needed, !diraction);
      delay(500);
      yield();
    }
  }
}

void setup() {
  declare_variables();
  server_setup();

  pinMode(motor1DirPin, OUTPUT);
  pinMode(motor2DirPin, OUTPUT);

  pinMode(motor1EnPin, OUTPUT);
  pinMode(motor2EnPin, OUTPUT);

  pinMode(motor1StepPin, OUTPUT);
  pinMode(motor2StepPin, OUTPUT);

  digitalWrite(motor1EnPin, HIGH);
  digitalWrite(motor2EnPin, HIGH);
}

void loop() {
  server_loop();
}

void runMotor(int motorNo, int totSteps, bool direction) {

  Serial.print("Running Motor: "); Serial.println(motorNo + 1);
  Serial.print("Total Steps: "); Serial.println(totSteps);
  Serial.print("Dirrection: "); Serial.println(direction);

  if (motorNo == Motor1) {

    digitalWrite(motor1EnPin, LOW);
    digitalWrite(motor1DirPin, direction);
    for (int i = 0 ; i < totSteps; i++) {
      digitalWrite(motor1StepPin, HIGH);
      delayMicroseconds(delayBetweenPulse);
      digitalWrite(motor1StepPin, LOW);
      delayMicroseconds(delayBetweenPulse);
      yield();
    }

    digitalWrite(motor1EnPin, HIGH);
  }
  else if (motorNo == Motor2) {
    digitalWrite(motor2EnPin, LOW);
    digitalWrite(motor2DirPin, direction);
    for (int i = 0 ; i < totSteps; i++) {
      digitalWrite(motor2StepPin, HIGH);
      delayMicroseconds(delayBetweenPulse2);
      digitalWrite(motor2StepPin, LOW);
      delayMicroseconds(delayBetweenPulse2);
      yield();
    }
    digitalWrite(motor2EnPin, HIGH);
  }
  
}

void test() {
  digitalWrite(motor1DirPin, LOW);
  digitalWrite(motor1EnPin, LOW);

  for (int i = 0 ; i < 100; i++) {
    digitalWrite(motor1StepPin, HIGH);
    delayMicroseconds(delayBetweenPulse);
    digitalWrite(motor1StepPin, LOW);
    delayMicroseconds(delayBetweenPulse);
    yield();
  }
  Serial.println("Motor 1 forward rotation finished!!");
  digitalWrite(motor1EnPin, HIGH);

  delay(5000);


  digitalWrite(motor1EnPin, LOW);
  digitalWrite(motor1DirPin, HIGH);
  for (int i = 0 ; i < 100; i++) {
    digitalWrite(motor1StepPin, HIGH);
    delayMicroseconds(delayBetweenPulse);
    digitalWrite(motor1StepPin, LOW);
    delayMicroseconds(delayBetweenPulse);
    yield();
  }
  Serial.println("Motor 1 backword rotation finished!!");
  digitalWrite(motor1EnPin, HIGH);

  delay(2000);

  digitalWrite(motor2EnPin, LOW);
  digitalWrite(motor2DirPin, LOW);
  for (int i = 0 ; i < 100; i++) {
    digitalWrite(motor2StepPin, HIGH);
    delayMicroseconds(delayBetweenPulse2);
    digitalWrite(motor2StepPin, LOW);
    delayMicroseconds(delayBetweenPulse2);
    yield();
  }

  Serial.println("Motor 2 forward rotation finished!!");
  digitalWrite(motor2EnPin, HIGH);

  delay(5000);

  digitalWrite(motor2EnPin, LOW);
  digitalWrite(motor2DirPin, HIGH);
  for (int i = 0 ; i < 100; i++) {
    digitalWrite(motor2StepPin, HIGH);
    delayMicroseconds(delayBetweenPulse2);
    digitalWrite(motor2StepPin, LOW);
    delayMicroseconds(delayBetweenPulse2);
    yield();
  }
  digitalWrite(motor2EnPin, HIGH);

  Serial.println("Motor 2 backward rotation finished!!");
}
