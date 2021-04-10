/*
 By Liyanboy74
 https://github.com/liyanboy74
*/

#include <mega16.h>
#include <delay.h>

#include "key.h"

char getkey()
{
char ch='?';
KEYPAD_DDR=0b11110000;
KEYPAD_PORT=0b11111111;

while(ch=='?')
 {
 KEYPAD_PORT.4=0;
 delay_us(10);
      if(KEYPAD_PIN.0==0)ch='1'; 
 else if(KEYPAD_PIN.1==0)ch='4';
 else if(KEYPAD_PIN.2==0)ch='7';
 else if(KEYPAD_PIN.3==0)ch='*';
 KEYPAD_PORT.4=1;

 KEYPAD_PORT.5=0;
 delay_us(10);
      if(KEYPAD_PIN.0==0)ch='2';
 else if(KEYPAD_PIN.1==0)ch='5';
 else if(KEYPAD_PIN.2==0)ch='8';
 else if(KEYPAD_PIN.3==0)ch='0';
 KEYPAD_PORT.5=1;

 KEYPAD_PORT.6=0;
 delay_us(10);
      if(KEYPAD_PIN.0==0)ch='3';
 else if(KEYPAD_PIN.1==0)ch='6';
 else if(KEYPAD_PIN.2==0)ch='9';
 else if(KEYPAD_PIN.3==0)ch='#';
 KEYPAD_PORT.6=1; 
 
 KEYPAD_PORT.7=0;
 delay_us(10);
      if(KEYPAD_PIN.0==0)ch='A';
 else if(KEYPAD_PIN.1==0)ch='B';
 else if(KEYPAD_PIN.2==0)ch='C';
 else if(KEYPAD_PIN.3==0)ch='D';
 KEYPAD_PORT.7=1;    
 } 
KEYPAD_PORT=0b0001111;
delay_ms(1); 
while((KEYPAD_PIN&0b1111)!=0b1111);
delay_ms(50);
return(ch);
}


char chek_sharp()
{
KEYPAD_DDR= 0b00001111;
KEYPAD_PORT=0b11110111;
delay_ms(1);
if(KEYPAD_PIN.6==0)return 1;
else return 0;
}


