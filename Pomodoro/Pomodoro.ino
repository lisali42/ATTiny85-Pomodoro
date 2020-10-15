#include "TM1637.h"

#define CLK 1
#define DIO 0
#define BTN 2
#define ARROWBTN A2

TM1637 tm1637(CLK,DIO);
volatile boolean pause=0;

ISR(INT0_vect) {
  while(digitalRead(BTN)){
  }
  pause=~pause;
}

void setup() {
  // put your setup code here, to run once:
  tm1637.set();
  tm1637.init();
  MCUCR |= 0b01000000;
  pinMode(ARROWBTN, INPUT);
  MCUCR |= 0x3;  // Rising EDGE
  GIMSK |= 1<<INT0;  // Activate the INT0
  sei();             //Active General Interrupt
}

void loop() {
  unsigned int focusMins=0;
  unsigned int focusSeconds=0;
  boolean timerUp=0;
  byte setFocus = 0x01; //0b0000_0001 set 10s minutes, 0b0000_0010 minutes, 0b0000_0100 10s seconds, 0b0000_1000 seconds  
  
  while(pause==0){
    //SW1:1023, SW2:660, SW3:440, SW4:330
    //need to set an interupt to read the analog pin
    if(setFocus == 0x01){ //set 10s minutes
      if((analogRead(ARROWBTN)/1000)==1){
        //nothing
      }else if((analogRead(ARROWBTN)%1000)/100 == 6){
        if(focusMins/10 == 9){
          focusMins -= 90;
        }else{
          focusMins += 10;
        }
      }else if((analogRead(ARROWBTN)%1000/100) == 4){
        if(focusMins/10 == 0){
          focusMins += 90;
        }else{
          focusMins -= 10;
        }
      }else if((analogRead(ARROWBTN)%1000/100) == 3){
        setFocus=setFocus<<1;
      }
        //Below is to make the first digit flash
        delay(500);
        tm1637.display(0x00,0x7f);
        delay(500);
    }else if(setFocus == 0x02){
      if((analogRead(ARROWBTN)/1000)==1){
        setFocus = setFocus >> 1;
      }else if((analogRead(ARROWBTN)%1000)/100 == 6){
        if(focusMins%10 == 9){
          focusMins -= 9;
        }else{
          focusMins += 1;
        }
      }else if((analogRead(ARROWBTN)%1000/100) == 4){
        if(focusMins%10 == 0){
          focusMins += 9;
        }else{
          focusMins -= 1;
        }
      }else if((analogRead(ARROWBTN)%1000/100) == 3){
        setFocus=setFocus<<1;
      }
        //Below is to make the first digit flash
        delay(500);
        tm1637.display(0x01,0x7f);
        delay(500);
    }
    tm1637.display(0,focusMins/10);
    tm1637.display(1,focusMins%10);
    tm1637.display(2,focusSeconds/10);
    tm1637.display(3,focusSeconds%10);
  }
  
  while(!timerUp){
    if(!pause){
      tm1637.display(0,focusMins/10);
      tm1637.display(1,focusMins%10);
      tm1637.display(2,focusSeconds/10);
      tm1637.display(3,focusSeconds%10);
      tm1637.point(POINT_ON);
      if(focusSeconds==0){
        focusMins--;
        focusSeconds=59;
      }
      else{
        focusSeconds--;
      }
      if(focusSeconds==0&&focusMins==0){
        timerUp=1;
      }
      delay(1000);
    }
  }    
}
