/* Ramp accelerate a stepper motor up to a constant speed and hold it at that speed 
until a predefined period of time has ended or until a button is pressed to indicate 
a desire to slow down
*/

#include <AccelStepper.h>

// Define a stepper and the pins it will use
AccelStepper stepper(1,7,6); //1 is driving mode, 7 is pulse, 6 is direction

int targSpeed = 3000; // The target speed in steps/sec (maximum seems to be around 1850 for full steps with no load)
float currSpeed = 0; // current speed in steps/sec
float accel = 500; //acceleration limit in steps/sec/sec

unsigned long t_prev;
unsigned long t_curr;
unsigned long dt;
unsigned long t_run = 2000; //maximum run time in milliseconds

void setup()
{  

  Serial.begin(9600);
  Serial.println("-----------------------------------------");

  // This is my short term method of setting the other driver pins to the correct value. This should be replaced by hardwire to voltage supply in the future.
  pinMode(5,OUTPUT);
  pinMode(4,OUTPUT);
  digitalWrite(5,HIGH);
  digitalWrite(4,HIGH);

  // Change these to suit your stepper if you want
  //stepper.setAcceleration(50); // This value doesnt actually work with speed control, only position control.
  stepper.setMaxSpeed(10000); // determining the max speed experimentally
  stepper.setSpeed(0); // initial speed is zero

  delay(5000);
  t_curr = millis();
}

void loop()
{
  while(currSpeed<targSpeed){
    //every instance increase the currSpeed proportionally to the time passed since the last instance then update the setspeed command
    t_prev = t_curr;
    stepper.runSpeed();
    t_curr = millis();
    stepper.runSpeed();
    dt = (t_curr-t_prev);
    stepper.runSpeed();

    currSpeed = currSpeed+accel*(float)dt/1000; //to calculate the new speed do the math under type float then convert to integer later. We do this because if we calculate under type integer the while loop is too fast and the currSpeed doesnt update
    stepper.runSpeed();
    stepper.setSpeed(currSpeed);
    stepper.runSpeed();
  }

  Serial.println("Target speed reached");
  stepper.setSpeed(targSpeed);
  t_prev = millis();

  while(dt<t_run){ //while the designated spin time has not been breached
    //check to see if button has been pressed
    //STILL TO DO

    //set speed
    stepper.runSpeed();

    //calculate dt
    dt = millis()-t_prev;
  }

  Serial.println("Decelerating");
  stepper.runSpeed();
  
  t_curr = millis();

  while(currSpeed>0){
    //every instance decrease the currSpeed proportionally to the time passed since the last instance then update the setspeed command
    //Serial.println(currSpeed);
    t_prev = t_curr;
    stepper.runSpeed();
    t_curr = millis();
    stepper.runSpeed();
    dt = (t_curr-t_prev);
    stepper.runSpeed();

    currSpeed = currSpeed-accel*(float)dt/1000;
    stepper.runSpeed(); //to calculate the new speed do the math under type float then convert to integer later. We do this because if we calculate under type integer the while loop is too fast and the currSpeed doesnt update

    stepper.setSpeed(currSpeed);
    stepper.runSpeed();
    
  }

  
  stepper.setSpeed(500);
  stepper.runSpeed();

  while(true){ //pause here until reset button is pressed NOT YET IMPLEMENTED

  }

}
