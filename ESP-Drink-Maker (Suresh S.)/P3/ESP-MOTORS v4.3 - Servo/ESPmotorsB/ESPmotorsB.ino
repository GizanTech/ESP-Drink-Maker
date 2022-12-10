#include "ESPmotorsB.h"
#include "AccelStepper.h"
#include <Servo.h>

#define spdSrv 2 //increase to make servo slow

#define Motor1 0
#define Motor2 1
#define motorInterfaceType 1

//Pin defining


#define servMotorPin 2
#define holdAngle 90
#define releaseAngle 0
#define holdTime 5000
Servo servo;

#define motor1DirPin 15
#define motor1EnPin 16
#define motor1StepPin 13

//Switches
#define limit_switch1 14   // This is GPIO 14 on ESP - D5
#define limit_switch2 12   // This is GPIO 12 on ESP - D6
#define stop_switch 2

#define motor1_max_speed 1000.0 //steps/seconds
#define motor1_acceleration 200.0
#define motor1_initial_speed 500.0 // start speed


AccelStepper stepper1(motorInterfaceType, motor1StepPin, motor1DirPin);


int motor1_limit = 0; //Ignore limit switch. To use limit switch, make the value 1.
int motor2_limit = 0; //Ignore limit switch. To use limit switch, make the value 1.

#define oneStepDistance 0.2 //We need to calibrate this one.

#define number_of_bottles 21
#define number_of_drinks 20

long bottle_distance[number_of_bottles + 1];
long bottle_height[number_of_bottles + 1];
long bottle_holdAngle[number_of_bottles + 1];
long bottle_releaseAngle[number_of_bottles + 1];

int currBottle = 0;

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

  servo.attach(servMotorPin);
  servo.write(releaseAngle);

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
    
    currBottle = drink_combination[drink][i];
    
    if (currBottle == -1) {
      break;
    }

    long destination = bottle_distance[currBottle];
    Serial.print("Destination X: "); Serial.println(destination);
    long distance = destination - current_position;
    float steps_needed = distance * 1.0 / oneStepDistance;
    current_position = destination;
    Serial.print("steps_needed X: "); Serial.println(steps_needed);

    runMotor(Motor1, steps_needed);
    delay(500);
    
    destination = bottle_height[currBottle];
    distance = destination;
    Serial.print("Destination Y: "); Serial.println(destination);
    steps_needed = distance * 1.0 / oneStepDistance;
    Serial.print("steps_needed Y: "); Serial.println(steps_needed);

    runMotor(Motor2, steps_needed);
    delay(500); // This is to delay the Servo from going up again too quickly
    uint32_t holdingTime = drink_hold_time[drink][i] * 1000;
    Serial.print("Holding Time: ");Serial.println(holdingTime);
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

    if (totSteps < 0) {
       for(int ang = 0 ; ang < bottle_releaseAngle[currBottle]; ang++){
        servo.write(ang);
        delay(spdSrv);
      }
    }
    else {
      servo.write(bottle_holdAngle[currBottle]); 
    }
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
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 2;
  bottle_distance[bottle_num] = -1790 ; //mm
  bottle_height[bottle_num] = -164; //mm
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 3;
  bottle_distance[bottle_num] = -1691; //mm
  bottle_height[bottle_num] = -164; //mm
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 4;
  bottle_distance[bottle_num] = -1588; //   300 mm
  bottle_height[bottle_num] = -163; //mm  ISSUES
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 5;
  bottle_distance[bottle_num] = -1495; //mm
  bottle_height[bottle_num] = -162; //mm ISSUES
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 6;
  bottle_distance[bottle_num] = -1392; //mm
  bottle_height[bottle_num] = -157; //mm
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 7;
  bottle_distance[bottle_num] = -1294; //mm
  bottle_height[bottle_num] = -155; //mm ISSUES
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 8;
  bottle_distance[bottle_num] = -1190; //mm
  bottle_height[bottle_num] = -156; //mm
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 9;
  bottle_distance[bottle_num] = -1095; // 793 mm
  bottle_height[bottle_num] = -156; //mm
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 10;
  bottle_distance[bottle_num] = -995; //893 mm
  bottle_height[bottle_num] = -156; //mm
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 11;
  bottle_distance[bottle_num] = -896; // 992 mm
  bottle_height[bottle_num] = -157; //mm
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 12;
  bottle_distance[bottle_num] = -793; // 1095 mm
  bottle_height[bottle_num] = -158; //mm
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 13;
  bottle_distance[bottle_num] = -699; // 1189 mm
  bottle_height[bottle_num] = -136; //mm
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 14;
  bottle_distance[bottle_num] = -599; // 1289 mm
  bottle_height[bottle_num] = -137; //mm
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 15;
  bottle_distance[bottle_num] = -502; //mm
  bottle_height[bottle_num] = -135; //mm
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 16;
  bottle_distance[bottle_num] = -400; //1488 mm
  bottle_height[bottle_num] = -136; //mm
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 17;
  bottle_distance[bottle_num] = -297; //mm
  bottle_height[bottle_num] = -136; //mm
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 18;
  bottle_distance[bottle_num] = -201; //1687 mm
  bottle_height[bottle_num] = -137; //mm
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 19;
  bottle_distance[bottle_num] = 1787; //mm
  bottle_height[bottle_num] = -32; //mm
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 20;
  bottle_distance[bottle_num] = 0; //1888 mm
  bottle_height[bottle_num] = -32; //mm
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;

  bottle_num = 21;
  bottle_distance[bottle_num] = 0; //1888 mm
  bottle_height[bottle_num] = +50; //mm
  bottle_holdAngle[bottle_num] = 90;
  bottle_releaseAngle[bottle_num] = 0;
}
