#include "AccelStepper.h"

#define motorInterfaceType 1
#define motor1StepPin      2
#define motor1DirPin       3
#define motor1EnPin        4

AccelStepper stepper1(motorInterfaceType, motor1StepPin, motor1DirPin);

#define motor1_max_speed 1500.0 //steps/seconds
#define motor1_acceleration 300.0
#define motor1_initial_speed 500.0 // start speed

#define Motor1 0

long current_position = 0;

void setup() {

  pinMode(motor1EnPin, OUTPUT);
  digitalWrite(motor1EnPin, HIGH);

  stepper1.setMaxSpeed(motor1_max_speed);
  stepper1.setAcceleration(motor1_acceleration);
  stepper1.setSpeed(motor1_initial_speed);

// Testing the motor
  test();
}

void loop() {

}


void runMotor(long motorNo, long totSteps) {
  if (motorNo == Motor1) {
    digitalWrite(motor1EnPin, LOW);
    stepper1.move(totSteps);
    while (stepper1.distanceToGo() != 0) {
      stepper1.run();
      yield();
    }
    digitalWrite(motor1EnPin, HIGH);
  }
}

void test() {
  Serial.println("Motor 1 forward rotation started!!");
  runMotor(Motor1, -5000);
  Serial.println("Motor 1 forward rotation finished!!");
  
  
  Serial.println("Motor 1 backword rotation started!!");
  runMotor(Motor1, +5000);
  Serial.println("Motor 1 backword rotation finished!!");
}
