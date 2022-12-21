#include "mbed.h"
#include "bbcar.h"

Ticker servo_ticker;
Ticker servo_feedback_ticker;

PwmIn servo0_f(D12), servo1_f(D11); // servo0 - left ; servo1 - right
PwmOut servo0_c(D9), servo1_c(D10); 
BBCar car(servo0_c, servo0_f, servo1_c, servo1_f, servo_ticker, servo_feedback_ticker);
EventQueue distanceQ;
Thread measureThread;


BusInOut qti_pin(D4,D5,D6,D7);

 
float angle;
float PI = 3.1415926535897932;

void distance_travelled() {
    float distance; 
    angle = (car.readAngle0() + car.readAngle1()) / 2; // taking the average from two wheels
    distance = (6.5*PI*angle) / 360;
    printf(" angle = %f\n", angle);
}

int main() {

   measureThread.start(callback(&distanceQ, &EventQueue::dispatch_forever));
   parallax_qti qti1(qti_pin);
   int pattern;

   car.goStraight(50);
   while(1) {
      pattern = (int)qti1;
      printf("%d\n",pattern);

      switch (pattern) {
         case 0b1000: car.turn(90, 0.1); break;
         case 0b1100: car.turn(90, -0.3); break;
         case 0b0100: car.turn(50, -0.7); break;
         case 0b0110: car.goStraight(30); break;
         case 0b0010: car.turn(50, 0.7); break;
         case 0b0011: car.turn(90, 0.3); break;
         case 0b0001: car.turn(90, 0); break;
         case 0b1111: {
            car.stop();
            distanceQ.call(&distance_travelled);
            break;}
         default: car.goStraight(50);
      }
      ThisThread::sleep_for(10ms);
   }
}