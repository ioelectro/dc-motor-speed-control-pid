/*
 By Liyanboy74
 https://github.com/liyanboy74
*/

#include <mega16.h>
#include <alcd.h>
#include <delay.h>
#include <stdio.h>

#include "key.h"

//coefficient of PID Controler
float kp =0.5;
float kd =0.01;
float ki =0.02;

float A=0.0,B=0.0,C=0.0;

// error value
int err=0,lerr=0;

// controller output value
long int ctrl;

long int input=0;   //Input
long int p=0;       //Pulse Counter
long int speed=0;   //Speed

// External Interrupt 0 service routine
interrupt [EXT_INT0] void ext_int0_isr(void)
{
    //Pulse Counter
	p++;
}


// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
    //Calculate Speed
    //The coefficient depends on the encoder used
	speed=p*45.776;

    //Reset Pulse Counter for Nex Time
	p=0;
}

void main(void)
{
// Buffer
char ch,str[32];

// Input/Output Ports initialization
DDRD.5=1;
DDRD.6=1;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 7.813 kHz
// Mode: Normal top=0xFF
// OC0 output: Disconnected
// Timer Period: 32.768 ms
TCCR0=(0<<WGM00) | (0<<COM01) | (0<<COM00) | (0<<WGM01) | (1<<CS02) | (0<<CS01) | (1<<CS00);
TCNT0=0x00;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 8000.000 kHz
// Mode: Fast PWM top=ICR1
// OC1A output: Non-Inverted PWM
// OC1B output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer Period: 0.12513 ms
// Output Pulse(s):
// OC1A Period: 0.12513 ms Width: 0 us
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=(1<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (1<<WGM11) | (0<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (1<<WGM13) | (1<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x03;
ICR1L=0xE8;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (1<<TOIE0);

// External Interrupt(s) initialization
// INT0: On
// INT0 Mode: Rising Edge
// INT1: Off
// INT2: Off
GICR|=(0<<INT1) | (1<<INT0) | (0<<INT2);
MCUCR=(0<<ISC11) | (0<<ISC10) | (1<<ISC01) | (1<<ISC00);
MCUCSR=(0<<ISC2);
GIFR=(0<<INTF1) | (1<<INTF0) | (0<<INTF2);

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud Rate: 9600
UCSRA=(0<<RXC) | (0<<TXC) | (0<<UDRE) | (0<<FE) | (0<<DOR) | (0<<UPE) | (0<<U2X) | (0<<MPCM);
UCSRB=(0<<RXCIE) | (0<<TXCIE) | (0<<UDRIE) | (1<<RXEN) | (1<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);
UCSRC=(1<<URSEL) | (0<<UMSEL) | (0<<UPM1) | (0<<UPM0) | (0<<USBS) | (1<<UCSZ1) | (1<<UCSZ0) | (0<<UCPOL);
UBRRH=0x00;
UBRRL=0x33;

// Alphanumeric LCD initialization
// Connections are specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
lcd_init(16);

// Global enable interrupts
#asm("sei")

while (1)
      {
      //Get Speed from user
      lcd_clear();
      lcd_puts("Enter RPM:");
      sprintf(str,"%ld",input);
      lcd_gotoxy(0,1);
      lcd_puts(str);
      ch=getkey();

      //Reset input and output
      if(ch=='#')
       {
       input=0;
       OCR1A=0;
       }

      //Calculate Entered Speed by user
      else if(ch!='A'&&ch!='B'&&ch!='C'&&ch!='D'&&ch!='*')
       {
       if(input<10000)
       input=(input*10)+(ch-'0');
       }

      //Run PID Controler
      else if(ch=='*')
       {
       //Range check
       if(input>100000)input=100000;
       else if(input<0)input=0;

       //Reset memory
       OCR1A=0;
       lerr=0;
       err=0;
       ctrl=0;
       A=0;
       B=0;
       C=0;

       //Clear LCD
       lcd_clear();

       //Loop - Break by holding # key
       while(1)
        {
        //Display Values on LCD
        lcd_gotoxy(0,0);
        lcd_puts("Runing... ");
        sprintf(str,"%07ld->%07ld    ",speed,input);
        lcd_gotoxy(0,1);
        lcd_puts(str);

        //Controller Process
        err = input - speed;
        A=kp*err;
        B=(ki*err)+B;
        C=(err-lerr)*kd;
        lerr=err;

        //Output Signal
        ctrl=A+B+C;

        //Range check
        if(ctrl < 0) ctrl = 0;
        else if(ctrl > 1000) ctrl = 1000;

        //Generate pwm
        OCR1A= ctrl;

        delay_ms(10);

        if(chek_sharp()==1)
         {
         //Stop PWM
         OCR1A=0;

         lcd_clear();
         lcd_puts("Stoping...");
         while(chek_sharp()==1);
         delay_ms(500);
         break;
         }
        }
       }

      delay_ms(200);
      }
}
