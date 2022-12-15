#include "ESPmotorsB.h"
#include "AccelStepper.h"

#define limit_switch1 14    // This is GPIO 14 on ESP - D5
#define limit_switch2 12    // This is GPIO 12 on ESP - D6
#define stop_switch   2     // This is GPIO  2 on ESP - D4
#define motor1EnPin   16    // This is GPIO 16 on ESP - D0
#define motor1StepPin 13    // This is GPIO 13 on ESP - D7
#define motor1DirPin  15    // This is GPIO 15 on ESP - D8
#define lineMotorEN   2     // This is GPIO  2 on ESP - D4
#define lineMotorIN1  5     // This is GPIO  5 on ESP - D1
#define lineMotorIN2  16    // This is GPIO 16 on ESP - D0

#define spdOfLineMotor 30 //mm/sec

#define motorInterfaceType 1
AccelStepper stepper1(motorInterfaceType, motor1StepPin, motor1DirPin);

#define motor1_max_speed 1500.0 //steps/seconds
#define motor1_acceleration 300.0
#define motor1_initial_speed 500.0 // start speed

#define Motor1 0
#define Motor2 1

long current_position = 0;
uint32_t  stop_time = 2000;
uint32_t  stop_time_start;

bool motor1_limit = false; //Ignore limit switch. To use limit switch activate this variable in the code.
bool motor2_limit = false; //Ignore limit switch. To use limit switch activate this variable in the code.

void setup() {
  declare_variables();
  server_setup();

  pinMode(limit_switch1, INPUT_PULLUP); // GPIO 14 - D5 on ESP
  pinMode(limit_switch2, INPUT_PULLUP);
  pinMode(stop_switch,   INPUT_PULLUP);

  pinMode(motor1EnPin, OUTPUT);
  digitalWrite(motor1EnPin, HIGH);

  stepper1.setMaxSpeed(motor1_max_speed);
  stepper1.setAcceleration(motor1_acceleration);
  stepper1.setSpeed(motor1_initial_speed);

  pinMode(lineMotorEN, OUTPUT);
  pinMode(lineMotorIN1, OUTPUT);
  pinMode(lineMotorIN2, OUTPUT);

  //test();// This is to test directly after pushing code automatically without wifi
}

void loop() {
  server_loop();
}

void getDrink(long drink) {
  current_position = 0;
  Serial.print("Preparing  no: "); Serial.println(drink);

  if (drink == 51) {    // This is to test directly after pushing code using ESP wifi
    test();
    return;
  }

  // This is where all instructions start for drinks:
  // Add these if we add a dispenser
  // Replace step needed by the exact steps that we want it to go up before start.
  // runMotor(Motor2, steps_needed);

  for (long i = 0 ; i < 20; i++) {
    if (drink_combination[drink][i] == -1) {
      break;
    }

    long destination = bottle_distance[drink_combination[drink][i]];
    long distance = destination - current_position;
    float steps_needed = distance * 1.0 / oneStepDistance;
    current_position = destination;
    Serial.print("Destination  X:\t"); Serial.println(destination);
    Serial.print("steps_needed X:\t"); Serial.println(steps_needed);

    runMotor(Motor1, steps_needed);
    delay(500);

    destination = bottle_height[drink_combination[drink][i]];
    distance = destination;
    steps_needed = distance * 1.0 / spdOfLineMotor;
    Serial.print("Destination  Y:\t"); Serial.println(destination);
    Serial.print("steps_needed Y:\t"); Serial.println(steps_needed);

    runMotor(Motor2, steps_needed);
    uint32_t holdingTime = drink_hold_time[drink][i] * 1000;
    delay(holdingTime);

    steps_needed = -1 * steps_needed;
    runMotor(Motor2, steps_needed);
    delay(500);
    yield();
  }
  // The code below is the end step after the 20 instructions:
  // returning to home position...
  motor1_limit = true;
  long destination = 0;


  long distance = destination - current_position;
  long steps_needed = distance * 1.0 / oneStepDistance;
  current_position = destination;
  Serial.print("Destination X: FinalPosition"); Serial.println(destination);
  Serial.print("steps_needed X: "); Serial.println(steps_needed);

  // steps_needed = -1 * steps_needed * 2; // add twice the distance if using a limit switch

  runMotor(Motor1, steps_needed);
  motor1_limit = false;
}

void runMotor(long motorNo, long totSteps) {

  Serial.print("Running Motor: "); Serial.println(motorNo + 1);
  Serial.print("Total Steps/Time: "); Serial.println(totSteps);

  if (motorNo == Motor1) {
    digitalWrite(motor1EnPin, LOW);
    stepper1.move(totSteps);
    while (stepper1.distanceToGo() != 0) {
      if (digitalRead(limit_switch1) == LOW && current_position == 0) {
        Serial.println("Limit switch pressed!");
        break;
      }

      if (digitalRead(stop_switch) == LOW) { // This is to break the loop if stop switch is pressed.
        stop_time_start = millis();
        stepper1.stop();
        break;
      }
      else if (millis() - stop_time_start < stop_time) { // Stopping and keeping everythings stopped for 2000 millisec.
        stepper1.stop();
        break;
      }

      stepper1.run();
      yield();
    }
    digitalWrite(motor1EnPin, HIGH);
  }
  else if (motorNo == Motor2) {
    Serial.println("Running Linear actuator");
    digitalWrite(lineMotorEN, HIGH);
    if (totSteps < 0) {
      digitalWrite(lineMotorIN1, HIGH);
      digitalWrite(lineMotorIN2, LOW);
    }
    else {
      digitalWrite(lineMotorIN1, LOW);
      digitalWrite(lineMotorIN2, HIGH);
    }

    uint32_t lineMotorPrevTime = millis();
    uint32_t lineMotorTotal = abs(totSteps) * 1000;
    while(millis() - lineMotorPrevTime < lineMotorTotal){
      if (digitalRead(limit_switch2) == LOW) {
        Serial.println("Limit switch 2 pressed!");
        break;
      }

      // This is to break the loop if stop switch is pressed.
      if (digitalRead(stop_switch) == LOW) {
        stop_time_start = millis();
        break;
      }
      else if (millis() - stop_time_start < stop_time) {
        break;
      }
      yield();
    }
    
    digitalWrite(lineMotorIN1, LOW);
    digitalWrite(lineMotorIN2, LOW);
    digitalWrite(lineMotorEN, LOW);
  }
}

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
