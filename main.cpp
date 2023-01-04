#include "mbed.h"
#include "bbcar.h"


Ticker servo_ticker;
Ticker servo_feedback_ticker;

Thread carThread(osPriorityHigh);
EventQueue car_queue;

Thread measureThread;
EventQueue measure_queue;

//Car
PwmIn servo0_f(D12), servo1_f(D11); // servo0 - left ; servo1 - right
PwmOut servo0_c(D9), servo1_c(D10); 
BBCar car(servo0_c, servo0_f, servo1_c, servo1_f, servo_ticker, servo_feedback_ticker);

// QTI sensors
BusInOut qti_pin(D4,D5,D6,D7);
parallax_qti qti1(qti_pin);

int pattern; 
int a; 
float inita0, inita1;
double d,s;
const float PI = 3.13159;
int i = 0;
int carSpeed = 50;

void carStatus(){
            //distance travelled
            double d0, d1;
            d0 = 6.5*PI*(abs(car.servo0.angle-inita0)/360);
            d1 = 6.5*PI*(abs(car.servo1.angle-inita1)/360);
            s = ( (d0+d1) / 2 ); // speed
            inita0 = car.servo0.angle; //update current wheel angle for next call
            inita1 = car.servo1.angle;
            d = d + s;   
            printf("Distance since mark %d: %.2f \n", i, s);
}

int main(){
    inita0 = car.servo0.angle; //update current wheel angle for next call
    inita1 = car.servo1.angle;
    car.goStraight(carSpeed);
      while(1){
          if (i == 4){
              break;
          }
          pattern = (int)qti1;
          switch (pattern) {
            case 0b1000: car.turn(carSpeed, -0.1); a = 0; break;
            case 0b1100: car.turn(carSpeed, -0.4); a = 0; break;
            case 0b0100: car.turn(carSpeed, -0.7); a = 0; break;
            case 0b0110: car.goStraight(carSpeed); a = 0; break;
            case 0b0010: car.turn(carSpeed, 0.7); a = 0; break;
            case 0b0011: car.turn(carSpeed, 0.4); a = 0; break;
            case 0b0001: car.turn(carSpeed, 0.1); a = 0; break;
            case 0b1111: { // FINISHED
                if (a==0){
                    car.stop();
                    ThisThread::sleep_for(1s);
                    carStatus();
                    a++; i++;
                    break;
                }
                if  (a==1 && i < 4){
                    car.goStraight(carSpeed);
                    break;
                }
                else {
                    car.stop();
                    carStatus();
                    break;
                }
                }
            }
      ThisThread::sleep_for(10ms);
    }
     printf("Total distance travelled: %.2f (cm) \n", d);
}






