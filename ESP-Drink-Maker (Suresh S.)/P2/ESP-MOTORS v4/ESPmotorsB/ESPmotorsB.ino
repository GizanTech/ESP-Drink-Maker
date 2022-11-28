#include "ESPmotorsB.h"
#include "AccelStepper.h"

#define Motor1 0
#define Motor2 1

//Pin defining
#define motor2EnPin 5
#define motor1EnPin 16

#define motor2DirPin 4
#define motor1DirPin 15

#define motor2StepPin 0
#define motor1StepPin 13

//Switches
#define limit_switch1 14   // This is GPIO 14 on ESP - D5
#define limit_switch2 12   // This is GPIO 12 on ESP - D6
#define stop_switch 2

#define motor1_max_speed 2000.0 //steps/seconds
#define motor2_max_speed 2000.0 //steps/seconds

#define motor1_acceleration 100.0
#define motor2_acceleration 100.0

#define motor1_initial_speed 500.0 // start speed
#define motor2_initial_speed 500.0 // start speed

#define motorInterfaceType 1

AccelStepper stepper1(motorInterfaceType, motor1StepPin, motor1DirPin);
AccelStepper stepper2(motorInterfaceType, motor2StepPin, motor2DirPin);

#define motor1_limit 0 //Ignore limit switch. To use limit switch, make the value 1.
#define motor2_limit 0 //Ignore limit switch. To use limit switch, make the value 1.

#define oneStepDistance 0.1 //We need to calibrate this one.

#define number_of_bottles 21
#define number_of_drinks 20

long bottle_distance[number_of_bottles + 1];
long bottle_height[number_of_bottles + 1];

// This defines the number of drinks it will mix
long drink_combination[number_of_drinks + 1][20] = {
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // ignore this drink
  { 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},      // drink-1 combination with bottle number
  { 1, 2, 1, 2, 2, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},         // drink-2 combination with bottle number
  { 1, 2, 1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // drink-3 combination with bottle number
  { 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, -1, -1, -1, -1, -1, -1, -1, -1},
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

// This defines the time it will hold dispenser on each bottle.(in seconds)
long drink_hold_time[number_of_drinks + 1][20] = {
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // ignore this drink
  { 4, 4, 4, 4, 4, 8, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},       // drink-1 Stirring has 4 seconds and dispense has 10 seconds
  { 4, 4, 4, 4, 4, 4, -1, -1, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, -1},         // drink-2 combination with bottle holding time
  { 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // drink-3 combination with bottle holding time
  { 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
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

long current_position = 0;

void setup() {
  declare_variables();
  server_setup();

  pinMode(limit_switch1, INPUT_PULLUP); // GPIO 14 - D6 on ESP
  pinMode(limit_switch2, INPUT_PULLUP);
  pinMode(stop_switch, INPUT_PULLUP);

  pinMode(motor1EnPin, OUTPUT);
  pinMode(motor2EnPin, OUTPUT);

  digitalWrite(motor1EnPin, HIGH);
  digitalWrite(motor2EnPin, HIGH);

  stepper1.setMaxSpeed(motor1_max_speed);
  stepper1.setAcceleration(motor1_acceleration);
  stepper1.setSpeed(motor1_initial_speed);

  stepper2.setMaxSpeed(motor2_max_speed);
  stepper2.setAcceleration(motor2_acceleration);
  stepper2.setSpeed(motor2_initial_speed);

  // This is to test directly after pushing code automatically without wifi
  //test();
}

void loop() {
  server_loop();
  if (digitalRead(stop_switch) == LOW) {
    runMotor(Motor1, 0);
    runMotor(Motor2, 0);
  }
}

void getDrink(long drink) {
  current_position = 0;
  Serial.print("Preparing Drink no: "); Serial.println(drink);

  if (drink == 51) {
    // This is to test directly after pushing code using ESP wifi
    test();
    return;
  }

  // This is where all instructions start for drinks:
  // Add these if we add a dispenser
  // Replace step needed by the exact steps that we want it to go up before start.
  //  runMotor(Motor2, steps_needed);

  //

  for (long i = 0 ; i < 20; i++) {
    if (drink_combination[drink][i] == -1) {
      break;
    }

    long destination = bottle_distance[drink_combination[drink][i]];
    Serial.print("Destination X: "); Serial.println(destination);
    long distance = destination - current_position;
    float steps_needed = distance * 1.0 / oneStepDistance;
    current_position = 0;
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

  // The code below is the end step after the 20 instructions:
  long destination = 0;
  Serial.print("Destination X: "); Serial.println(destination);

  long distance = destination - current_position;
  long steps_needed = distance * 1.0 / oneStepDistance;
  current_position = 0;
  steps_needed = -1 * steps_needed ;
  Serial.print("steps_needed X: "); Serial.println(steps_needed);

  // steps_needed = -1 * steps_needed * 2; // add twice the distance if using a limit switch

  runMotor(Motor1, steps_needed);

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
    
    if (stepper1.currentPosition() == totSteps) {
      stepper1.moveTo(0);
    }
    else if (stepper1.currentPosition() == 0) {
      stepper1.moveTo(totSteps);
    }

    while (stepper1.distanceToGo() != 0) {
      if (digitalRead(limit_switch1) == LOW) {
        break;
      }
      stepper1.run();
      Serial.print(stepper1.distanceToGo()); Serial.print(" ");
      yield();
    }
    Serial.println("\nStepper1 Stop");
    stepper1.stop();
    digitalWrite(motor1EnPin, HIGH);
  }
  else if (motorNo == Motor2) {
    digitalWrite(motor2EnPin, LOW);

    if (stepper2.currentPosition() == totSteps) {
      stepper2.moveTo(0);
    }
    else if (stepper2.currentPosition() == 0) {
      stepper2.moveTo(totSteps);
    }
    stepper2.moveTo(totSteps);
    
    while (stepper2.distanceToGo() != 0) {
      if (digitalRead(limit_switch2) == LOW) {
        break;
      }
      stepper2.run();
      Serial.print(stepper2.distanceToGo()); Serial.print(" ");
      yield();
    }
    Serial.println("\nStepper2 Stop");
    stepper2.stop();
    digitalWrite(motor2EnPin, HIGH);
  }

}
// This is for tesing the motor

void test() {
  runMotor(Motor1, 2000);
  Serial.println("Motor 1 forward rotation finished!!");

  runMotor(Motor1, -2000);
  Serial.println("Motor 1 backword rotation finished!!");

  //runMotor(Motor2, -400 );
  Serial.println("Motor 2 forward rotation finished!!");

  //runMotor(Motor2, 50);
  Serial.println("Motor 2 backward rotation finished!!");
}

void declare_variables() {
  long bottle_num;

  bottle_num = 1;
  bottle_distance[bottle_num] = 0; //mm
  bottle_height[bottle_num] = -72; //mm

  bottle_num = 2;
  bottle_distance[bottle_num] = 100 ; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 3;
  bottle_distance[bottle_num] = 200; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 4;
  bottle_distance[bottle_num] = 300; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 5;
  bottle_distance[bottle_num] = 395; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 6;
  bottle_distance[bottle_num] = 495; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 7;
  bottle_distance[bottle_num] = 595; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 8;
  bottle_distance[bottle_num] = 695; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 9;
  bottle_distance[bottle_num] = 790; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 10;
  bottle_distance[bottle_num] = 890; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 11;
  bottle_distance[bottle_num] = 990; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 12;
  bottle_distance[bottle_num] = 1090; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 13;
  bottle_distance[bottle_num] = 1185; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 14;
  bottle_distance[bottle_num] = 1285; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 15;
  bottle_distance[bottle_num] = 1385; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 16;
  bottle_distance[bottle_num] = 1485; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 17;
  bottle_distance[bottle_num] = 1585; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 18;
  bottle_distance[bottle_num] = 1685; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 19;
  bottle_distance[bottle_num] = 1780; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 20;
  bottle_distance[bottle_num] = 1880; //mm
  bottle_height[bottle_num] = -32; //mm

  bottle_num = 21;
  bottle_distance[bottle_num] = 1880; //mm
  bottle_height[bottle_num] = -15; //mm
}
