
/*
 Stepper Motor Control - one step at a time

 This program drives a unipolar or bipolar stepper motor.
 The motor is attached to digital pins 8 - 11 of the Arduino.

 The motor will step one step at a time, very slowly.  You can use this to
 test that you've got the four wires of your stepper wired to the correct
 pins. If wired correctly, all steps should be in the same direction.

 Use this also to count the number of steps per revolution of your motor,
 if you don't know it.  Then plug that number into the oneRevolution
 example to see if you got it right.

 Created 30 Nov. 2009
 by Tom Igoe

 */

#include <Stepper.h>

const int stepsPerRevolution = 1000;  // change this to fit the number of steps per revolution
// for your motor

bool motorRunning = false;

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 12, 10, 11, 9);

int stepCount = 0;         // number of steps the motor has taken

void setup() {
  // initialize the serial port:
  Serial.begin(9600);
  myStepper.setSpeed(200);
}

void loop() {
  while (Serial.available() > 0) { // Check if data is available to read
    char command = Serial.read(); // Read the incoming byte
    
    if (command == '1') { // If '1' is received, start the motor
      motorRunning = true;
    } else if (command == '0') { // If '0' is received, stop the motor
      motorRunning = false;
    }
  }   

  if(motorRunning) {
    // step one step:
    myStepper.step(1);
    Serial.print("steps:");
    Serial.println(stepCount);
    stepCount+=1;
    delay(0);
  } else {
    stepCount = 0;
  }
}

