#include "ESPmotorsB.h"
#include "AccelStepper.h"
#include <Servo.h>

//Uncomment any of the following to make that corresponding Motor work
//#define stepMotor
//#define servMotor
#define lineMotor

#define Motor1 0
#define Motor2 1
#define motorInterfaceType 1

//Pin defining
#ifdef stepMotor
#define motor2EnPin 5
#define motor2StepPin 0
#define motor2DirPin 4

#define motor2_max_speed 1500.0 //steps/seconds
#define motor2_acceleration 300.0
#define motor2_initial_speed 500.0 // start speed
AccelStepper stepper2(motorInterfaceType, motor2StepPin, motor2DirPin);

#elifdef lineMotor
#define lineMotorEN  4 //Enable goes to GPIO 4
#define lineMotorIN1 0 //Enable goes to GPIO 0
#define lineMotorIN2 2 //Enable goes to GPIO 2
#define lineMotorTime 5000 //5 seconds for vertical movement in one direction 

#elifdef servMotor

#define servMotorPin 2
#define holdAngle 90
#define releaseAngle 0
#define holdTime 5000
Servo servo;

#endif

#define motor1DirPin 15
#define motor1EnPin 16
#define motor1StepPin 13

//Switches
#define limit_switch1 14   // This is GPIO 14 on ESP - D5
#define limit_switch2 12   // This is GPIO 12 on ESP - D6
#define stop_switch 2

#define motor1_max_speed 1100.0 //steps/seconds
#define motor1_acceleration 300.0
#define motor1_initial_speed 400.0 // start speed


AccelStepper stepper1(motorInterfaceType, motor1StepPin, motor1DirPin);


int motor1_limit = 0; //Ignore limit switch. To use limit switch, make the value 1.
int motor2_limit = 0; //Ignore limit switch. To use limit switch, make the value 1.

#define oneStepDistance 0.2 //We need to calibrate this one.

#define number_of_bottles 21
#define number_of_drinks 20

long bottle_distance[number_of_bottles + 1];
long bottle_height[number_of_bottles + 1];

uint32_t  stop_time = 2000;
uint32_t  stop_time_start;

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

long current_position = 0;

void setup() {
  declare_variables();
  server_setup();

  pinMode(limit_switch1, INPUT_PULLUP); // GPIO 14 - D5 on ESP
  pinMode(limit_switch2, INPUT_PULLUP);
  pinMode(stop_switch, INPUT_PULLUP);

  pinMode(motor1EnPin, OUTPUT);

  digitalWrite(motor1EnPin, HIGH);

  stepper1.setMaxSpeed(motor1_max_speed);
  stepper1.setAcceleration(motor1_acceleration);
  stepper1.setSpeed(motor1_initial_speed);

#ifdef stepMotor`
  pinMode(motor2EnPin, OUTPUT);
  digitalWrite(motor2EnPin, HIGH);

  stepper2.setMaxSpeed(motor2_max_speed);
  stepper2.setAcceleration(motor2_acceleration);
  stepper2.setSpeed(motor2_initial_speed);
#elifdef lineMotor
  pinMode(lineMotorEN, OUTPUT);
  pinMode(lineMotorIN1, OUTPUT);
  pinMode(lineMotorIN2, OUTPUT);
  
#elifdef servMotor
  servo.attach(servMotorPin);
  servo.write(releaseAngle);
#endif

  // This is to test directly after pushing code automatically without wifi
  test();
}

void loop() {
  server_loop();
}

void getDrink(long drink) {
  current_position = 0;
  Serial.print("Preparing  no: "); Serial.println(drink);

  if (drink == 51) {
    // This is to test directly after pushing code using ESP wifi
    test();
    return;
  }

  // This is where all instructions start for drinks:
  // Add these if we add a dispenser
  // Replace step needed by the exact steps that we want it to go up before start.
  //  runMotor(Motor2, steps_needed);



  for (long i = 0 ; i < 20; i++) {
    if (drink_combination[drink][i] == -1) {
      break;
    }

    long destination = bottle_distance[drink_combination[drink][i]];
    Serial.print("Destination X: "); Serial.println(destination);
    long distance = destination - current_position;
    float steps_needed = distance * 1.0 / oneStepDistance;
    current_position = destination;
    Serial.print("steps_needed X: "); Serial.println(steps_needed);

    runMotor(Motor1, steps_needed);
    delay(500);

    destination = bottle_height[drink_combination[drink][i]];
    distance = destination;
    Serial.print("Destination Y: "); Serial.println(destination);
    steps_needed = distance * 1.0 / oneStepDistance;
    Serial.print("steps_needed Y: "); Serial.println(steps_needed);

    runMotor(Motor2, steps_needed);
    uint32_t holdingTime = drink_hold_time[drink][i] * 1000;
    delay(holdingTime);

    steps_needed = -1 * steps_needed;
    runMotor(Motor2, steps_needed);
    delay(500);
    yield();
  }
  //
  // The code below is the end step after the 20 instructions:
  long destination = 0;
  motor1_limit = true;
  Serial.print("Destination X: FinalPosition"); Serial.println(destination);

  long distance = destination - current_position;
  long steps_needed = distance * 1.0 / oneStepDistance;
  current_position = destination;
  //steps_needed = -1 * steps_needed ;
  Serial.print("steps_needed X: "); Serial.println(steps_needed);

  // steps_needed = -1 * steps_needed * 2; // add twice the distance if using a limit switch

  runMotor(Motor1, steps_needed);
  motor1_limit = false;

  // Once it reaches the limit switch of HORIZONTAL

  //  steps_needed = -1 * 500; // add any big number so that it reaches the vertical limit switch
  // runMotor(Motor2, steps_needed);
  // delay(500);

}

void runMotor(long motorNo, long totSteps) {

  Serial.print("Running Motor: "); Serial.println(motorNo + 1);
  Serial.print("Total Steps: "); Serial.println(totSteps);

  if (motorNo == Motor1) {
    digitalWrite(motor1EnPin, LOW);
    stepper1.move(totSteps);
    while (stepper1.distanceToGo() != 0) {
      if (digitalRead(limit_switch1) == LOW && current_position == 0) {
        Serial.println("Limit switch pressed!");
        break;
      }

      // This is to break the loop if stop switch is pressed.
      if (digitalRead(stop_switch) == LOW) {
        stop_time_start = millis();
        stepper1.stop();
        break;
      }
      else if (millis() - stop_time_start < stop_time) {
        stepper1.stop();
        break;
      }

      stepper1.run();
      yield();
    }
    digitalWrite(motor1EnPin, HIGH);
  }
  else if (motorNo == Motor2) {


#ifdef stepMotor
    digitalWrite(motor2EnPin, LOW);
    stepper2.move(totSteps);
    while (stepper2.distanceToGo() != 0) {
      // This is to break the loop if stop switch is pressed.
      if (digitalRead(limit_switch2) == LOW && current_position == 0) {
        break;
      }
      //Stop Switch code
      if (digitalRead(stop_switch) == LOW) {
        stop_time_start = millis();
        stepper2.stop();
        break;
      }
      else if (millis() - stop_time_start < stop_time) {
        stepper2.stop();
        break;
      }
      stepper2.run();
      yield();
    }
    digitalWrite(motor2EnPin, HIGH);

#elifdef lineMotor
    digitalWrite(lineMotorEN, HIGH);
    if (totSteps < 0) {
      digitalWrite(lineMotorIN1, HIGH);
      digitalWrite(lineMotorIN2, LOW);
    }
    else {
      digitalWrite(lineMotorIN1, LOW);
      digitalWrite(lineMotorIN2, HIGH);
    }
    uint32_t lineMotorCurr = millis();
    while (millis() - lineMotorCurr < lineMotorTime) {
      // This is to break the loop if stop switch is pressed.
      if (digitalRead(limit_switch2) == LOW && current_position == 0) {
        digitalWrite(lineMotorEN, LOW);
        break;
      }
      //Stop Switch code
      if (digitalRead(stop_switch) == LOW) {
        stop_time_start = millis();
        digitalWrite(lineMotorEN, LOW);
        break;
      }
      else if (millis() - stop_time_start < stop_time) {
        digitalWrite(lineMotorEN, LOW);
        break;
      }
      yield();
    }
    digitalWrite(lineMotorEN, LOW);

#elifdef servMotor
    if (totSteps < 0) {
      servo.write(releaseAngle);
    }
    else {
      servo.write(holdAngle);
    }

#endif

  }
}

// This is for tesing the motor

void test() {
  runMotor(Motor1, -1000);
  Serial.println("Motor 1 forward rotation finished!!");

  runMotor(Motor1, +1000);
  Serial.println("Motor 1 backword rotation finished!!");

  runMotor(Motor2, -100 );
  Serial.println("Motor 2 forward rotation finished!!");

  runMotor(Motor2, 100);
  Serial.println("Motor 2 backward rotation finished!!");
}

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
}
