/* 
Open Camera Slider 1.0
==========================================================================

This software is intended to be used with the Open Camera Slider project 
hardware found at http://store.opencameraslider.com. It can be modified
to work with other hardware.


OPEN SOURCE LICENSE
--------------------------------------------------------------------------

The MIT License (MIT)

Copyright (c) 2015 Kevin Kanzelmeyer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
----------------------------------------------------------------------------
 */
 
#include <RGBTools.h>
#include <Stepper.h>

// Pin Assignments
#define DIR_SW1 2
#define DIR_SW2 3
#define END_SW1 4
#define END_SW2 5
#define LED_G 6
#define LED_R 7
#define LED_B 8
#define SPEED A0

RGBTools rgb(LED_R, LED_G, LED_B, COMMON_CATHODE);
enum STATE {
  STOP,
  FORWARD,
  REVERSE,
};
STATE state;

/*-----------------------------------------------
 *  Motor parameters for the 28BYJ48 motor
 *-----------------------------------------------*/
// the number of steps the motor will take during the step() method
int motorSteps = 16; 
// gear ratio of your motor
const int gearRatio = 64;
// number of steps required for a complete revolution of the motor pinion
const int stepsPerRevolution = motorSteps*gearRatio;
// step counter
int stepCount = 0;

// create the stepper motor
Stepper myStepper(stepsPerRevolution, 10, 12, 11, 13);

// Other variables
int sensorReading = 0;
int motorSpeed = 0;

void setup() {
  state = STOP; // initialize in the STOP state (slider will not move on startup
  pinMode(DIR_SW1, INPUT_PULLUP);  // initialize forward switch
  pinMode(DIR_SW2, INPUT_PULLUP);  // initialize forward switch
  pinMode(END_SW1, INPUT_PULLUP);  // initialize the end stop switch
  pinMode(END_SW2, INPUT_PULLUP);  // initialize the end stop switch

//  Serial.begin(9600);
//  Serial.println("Open Camera Slider 1.0");
//  Serial.println("");
//  Serial.println("");
  
// This section of code will move the slider off the end stop switch 
// if one of the stop switches is engaged at startup. It reads the direction
// set by the direction switch and steps the motor in that direction until
// the end stop switch is disengaged.
  myStepper.setSpeed(3);
  while (!digitalRead(END_SW1)) {
    rgb.setColor(Color::RED);
    myStepper.step(-motorSteps);
  }
  while (!digitalRead(END_SW2)) {
    rgb.setColor(Color::RED);
    myStepper.step(motorSteps);
  }
}

void loop() {
  switch (state) {
    
    case STOP:
//        Serial.println("State = STOP");
        // Write all motor pins low to stop current draw
        myStepper.setSpeed(0);
        myStepper.step(0);
        digitalWrite(10, LOW);
        digitalWrite(11, LOW);
        digitalWrite(12, LOW);
        digitalWrite(13, LOW);
        
        while(state == STOP) {
        rgb.setColor(Color::GREEN);
            if ((digitalRead(END_SW1)) && (digitalRead(END_SW2))) {    // if the stop switch is not engaged
                if      (!digitalRead(DIR_SW1)) {state = FORWARD;} 
                else if (!digitalRead(DIR_SW2)) {state = REVERSE;} 
                else                            {state = STOP;}
            }
        }
        break;
          
    case FORWARD:
//        Serial.println("State = FORWARD");
        while(state == FORWARD) {
            rgb.setColor(Color::RED);
            sensorReading = analogRead(SPEED);
            motorSpeed = map(sensorReading, 0, 1023, 10, 1);
            myStepper.setSpeed(motorSpeed);
            if (motorSpeed > 0) {
                myStepper.step(motorSteps);
            }            
            if((!digitalRead(END_SW1)) || (!digitalRead(END_SW2)) || (digitalRead(DIR_SW1))) {
                state = STOP;
            }
            if(!digitalRead(DIR_SW2)) {
                state = REVERSE;
            }
        }
        break;
        
    case REVERSE:
//        Serial.println("State = REVERSE");
        while(state == REVERSE) {
            rgb.setColor(Color::RED);
            sensorReading = analogRead(SPEED);
            motorSpeed = map(sensorReading, 0, 1023, 10, 1);
            myStepper.setSpeed(motorSpeed);
            if (motorSpeed > 0) {
                myStepper.step(-motorSteps);
            }
            if(!digitalRead(DIR_SW1)) {
                state = FORWARD;
            }
            if((!digitalRead(END_SW1)) || (!digitalRead(END_SW2)) || (digitalRead(DIR_SW2))) {
                state = STOP;
            }
        }
        break;
        
       default: break;
  }
}


