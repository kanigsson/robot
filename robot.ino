#include "PS2Mouse.h"

// connect motor controller pins to Arduino digital pins
// motor one
int left_enable = 10;
int left_in1 = 9;
int left_in2 = 8;
// motor two
int right_enable = 5;
int right_in1 = 7;
int right_in2 = 6;
// sonar range finder
int trigger = 13;
int echo = 12;
#define MOUSE_DATA 3
#define MOUSE_CLOCK 4

int left_speed = 200;
int right_speed = 200;

enum motorkind { left, right };

void motor_drive (motorkind m, int speed) {
  int in1, in2, set1, set2, enable;
  if (speed == 0) {
    set1 = LOW;
    set2 = LOW;
  } else if (speed > 0) {
    set1 = HIGH;
    set2 = LOW;
  } else {
    set1 = LOW;
    set2 = HIGH;
    speed = - speed;
  }
  if (m == left) {
    in1 = left_in1;
    in2 = left_in2;
    enable = left_enable;
  } else {
    in1 = right_in1;
    in2 = right_in2;
    enable = right_enable;
  }
  digitalWrite(in1, set1);
  digitalWrite(in2, set2);
  analogWrite(enable, speed);
}

void straight(int speed) {
  motor_drive(left, speed);
  motor_drive(right, speed);
}

int measuredistance() {
   long duration, distance;
   digitalWrite(trigger, LOW);
   delayMicroseconds(2);
   digitalWrite(trigger, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigger, LOW);
   duration = pulseIn(echo, HIGH);
   distance = duration / 6;
   return distance;
}

PS2Mouse mouse(MOUSE_CLOCK, MOUSE_DATA, STREAM);

void setup()
{
Serial.begin(38400);  
  Serial.println("start");
  mouse.initialize();
  Serial.println("init");  
  // set all the motor control pins to outputs
  pinMode(left_enable, OUTPUT);
  pinMode(right_enable, OUTPUT);
  pinMode(left_in1, OUTPUT);
  pinMode(left_in2, OUTPUT);
  pinMode(right_in1, OUTPUT);
  pinMode(right_in2, OUTPUT);
  delay(50);
  straight(150);  
}

void adapt_speed(movement m) {
  Serial.println(m.x);
  if (m.x > 0) {
    if (left_speed + right_speed > 400) {
      right_speed -= m.x;
      motor_drive(right, right_speed);
    } else {
      left_speed += m.x;
      motor_drive(left, left_speed);
    }
  } else if (m.x < 0) {
    if (left_speed + right_speed > 400) {
      left_speed += m.x;
      motor_drive(left, left_speed);
    } else {
      right_speed -= m.x;
      motor_drive(right, right_speed);
    }
  }
}

void loop()
{
  adapt_speed(mouse.report());  
}
