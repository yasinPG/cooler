/*****************************************************
This program was produced by the
CodeWizardAVR V2.05.3 Standard
Automatic Program Generator
© Copyright 1998-2011 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 5/26/2018
Author  : mohammad taha
Company : world
Comments: 


Chip type               : ATmega32A
Program type            : Application
AVR Core Clock frequency: 8.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*****************************************************/

#include <mega32a.h>
#include <delay.h>
// 1 Wire Bus interface functions
#include <1wire.h>

// DS1820 Temperature Sensor functions
#include <ds18b20.h>
#include <stdio.h>
#include <delay.h>
#include <stdlib.h>

// Alphanumeric LCD functions
#include <alcd.h>
#include <nRF24L01+.h>

#define manual_control_butt PINA.1
#define manual_pump PINA.4
#define manual_motor PINA.3
#define manual_fast PINA.2

#define plus_butt PINA.6
#define minus_butt PINA.5

#define relay_pump PORTD.1
#define relay_motor PORTD.0
#define relay_fast PORTD.2


int i=0;
int j=0;
int k=0;
int L=0;
int slow=0;
eeprom int priority=3;
eeprom int tar_tmp=20;
int curr_tmp1, curr_tmp2, curr_tmp;
int cool=0;
int fast_start=1;
int first_time=1;
int tar_tmpov=0;
int tar_tmpovc=0;
char str1[33], str2[33];


// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
// Place your code here
TCNT0=6;    
i++;
k++;
L++;
if (k==13)  //0.104 Second timer
    {
    k=0;
    if (manual_control_butt==1)
        {
        if (minus_butt==1)
            {
            priority = priority - 1;
            if (priority>6 || priority<1)
                {
                priority = priority + 1;
                }
            }
        if (plus_butt==1)
            {
            priority = priority + 1;
            if (priority>6 || priority<1)
                {
                priority = priority - 1;
                }
            }
        }
    else
        {
        if (minus_butt==1)
            {
            cool=0;
            tar_tmp=tar_tmp-1; 
            j=0;
            fast_start=1;
            }
        if (plus_butt==1)
            {
            cool=0;
            tar_tmp=tar_tmp+1;
            j=0;
            fast_start=1;
            }
        }
    }
if (L==425)  //Soft start Cooler in Fast mode, after 3.4 seconds switch to slow mode, if needed
    {
    L=0;
    if (relay_fast==1 && slow==1)
        {
            relay_fast=0;
        } 
    }
if (i==60) //0.48 Second timer
    {
    i=0;
    //___________________________print
    lcd_clear();
    sprintf(str1, "T1:%02d%cC T2:%2d%cC", curr_tmp1, 223, curr_tmp2, 223);    
    sprintf(str2,"Tar:%02d%cC P:%01d(%02d)",tar_tmp, 223, priority, curr_tmp);   
    lcd_gotoxy(0,0);
    lcd_puts(str1);
    lcd_gotoxy(0,1);
    lcd_puts(str2);
    j++;
    tar_tmpov=tar_tmp+2;
    tar_tmpovc=tar_tmp+cool;
    if (manual_control_butt==0) 
        {   
        if (j==7 && fast_start==1)  //3.36 second timer
            {
            j=0;
            fast_start=0;
            if (first_time==1)
                {
                if (curr_tmp>tar_tmp) //if temperature is more than the desired temperature at the start of device, Start cooler in fast mode
                    {
                    relay_pump=1;
                    relay_motor=1;
                    relay_fast=1;
                    slow=0;
                    }
                else  //turn off motor if temperature is desireable
                    {
                    relay_pump=0;
                    relay_motor=0;
                    relay_fast=0;
                    cool=1;
                    first_time=0;
                    slow=0;    
                    }      
                }
            else
                {
                if (curr_tmp>tar_tmpovc) //if temperature is more than the desired temperature, Start cooler
                    {
                    if (curr_tmp>tar_tmpov) //if too hot, start cooler in fast mode
                        {
                        relay_pump=1;
                        relay_motor=1;
                        relay_fast=1;
                        slow=0;
                        }
                    else  //start cooler in slow mode
                        {
                        relay_pump=1;
                        relay_motor=1;
                        relay_fast=1;
                        slow=1;
                        L=0;
                        }
                    }
                else  //turn off motor if temperature is desireable
                    {
                    relay_pump=0;
                    relay_motor=0;
                    relay_fast=0;
                    cool=1;
                    first_time=0;
                    slow=0;    
                    }
                }
            }
        if (j==50) //24 Second timer
            {
            j=0;
            if (first_time==1)
                {
                if (curr_tmp>tar_tmp) //if temperature is more than the desired temperature at the start of device, Start cooler in fast mode
                    {
                    relay_pump=1;
                    relay_motor=1;
                    relay_fast=1;
                    slow=0;
                    }
                else  //turn off motor if temperature is desireable
                    {
                    relay_pump=0;
                    relay_motor=0;
                    relay_fast=0;
                    cool=1;
                    first_time=0;
                    slow=0;    
                    }      
                }
            else
                {
                if (curr_tmp>tar_tmpovc) //if temperature is more than the desired temperature, Start cooler
                    {
                    if (curr_tmp>tar_tmpov) //if too hot, start cooler in fast mode
                        {
                        relay_pump=1;
                        relay_motor=1;
                        relay_fast=1;
                        slow=0;
                        }
                    else  //start cooler in slow mode
                        {
                        relay_pump=1;
                        relay_motor=1;
                        relay_fast=1;
                        slow=1;
                        L=0;
                        }
                    }
                else  //turn off motor if temperature is desireable
                    {
                    relay_pump=0;
                    relay_motor=0;
                    relay_fast=0;
                    cool=1;
                    first_time=0;
                    slow=0;    
                    }
                }
            }
        }
    if (j>50)
    {
        j=0;
    }
    }
}

// Declare your global variables here

void main(void)
{
// Declare your local variables here

// Input/Output Ports initialization
// Port A initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTA=0x00;
DDRA=0x00;

// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTB=0x00;
DDRB=0x00;

// Port C initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTC=0x00;
DDRC=0x00;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x00;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 31.250 kHz
// Mode: Normal top=0xFF
// OC0 output: Disconnected
TCCR0=0x04;
TCNT0=6;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=0xFFFF
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x00;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=0xFF
// OC2 output: Disconnected
ASSR=0x00;
TCCR2=0x00;
TCNT2=0x00;
OCR2=0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
MCUCR=0x00;
MCUCSR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x01;

// USART initialization
// USART disabled
UCSRB=0x00;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

// ADC initialization
// ADC disabled
ADCSRA=0x00;

// SPI initialization
// SPI disabled
SPCR=0x00;

// TWI initialization
// TWI disabled
TWCR=0x00;

// 1 Wire Bus initialization
// 1 Wire Data port: PORTA
// 1 Wire Data bit: 0
// Note: 1 Wire port settings are specified in the
// Project|Configure|C Compiler|Libraries|1 Wire menu.
w1_init();

// Alphanumeric LCD initialization
// Connections are specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
// RS - PORTC Bit 0
// RD - PORTC Bit 1
// EN - PORTC Bit 2
// D4 - PORTC Bit 4
// D5 - PORTC Bit 5
// D6 - PORTC Bit 6
// D7 - PORTC Bit 7
// Characters/line: 16
lcd_init(16);

nRF_Config(1);
// Global enable interrupts
#asm("sei")

DDRD.0=1;
DDRD.1=1;
DDRD.2=1;

priority=3;
tar_tmp=20;
while (1) 
      {
      if (priority>6 || priority<1)
          {
          priority=3;
          }
      curr_tmp1 = ds18b20_temperature(0);    //temp1         
      if(State == 1)    //temp2
          {
          curr_tmp2 = payload[1];
          State = 0;
          }
      if (curr_tmp2==0)    //final temp
          {
          curr_tmp = curr_tmp1;
          }
      else
          {
          if (priority==1)
              {
              curr_tmp = (3*curr_tmp1 + curr_tmp2)/4;
              }
          if (priority==2)
              {
              curr_tmp = (2*curr_tmp1 + curr_tmp2)/3;
              }
          if (priority==3 || priority>5 || priority<1)
              {
              curr_tmp = (curr_tmp1 + curr_tmp2)/2;
              }
          if (priority==4)
              {
              curr_tmp = (curr_tmp1 + 2*curr_tmp2)/3;
              }
          if (priority==5)
              {
              curr_tmp = (curr_tmp1 + 3*curr_tmp2)/4;
              } 
          if (priority==6)
              {
              curr_tmp = curr_tmp2;
              }
          }
       
      // Place your code here                 
      if (manual_control_butt==1) //if manual control is issued
          {
          if (manual_pump==1) //if water pump is pressed
              {
              relay_pump=1;
              }
          else
              {
              relay_pump=0;
              }
          if (manual_motor==1)  //dokmeye motor ?
              {
              relay_motor=1;  
              }
          else
              {
              relay_motor=0;  //motor khamoosh
              }
          if (manual_fast==1)  //tond ?
              {
              relay_fast=1;  //tond
              }
          else
              {
              relay_fast=0; //kond
              }
          }
      }
}
