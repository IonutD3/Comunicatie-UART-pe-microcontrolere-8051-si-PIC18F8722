/*
 * 02 - UART 8051 cu intrerupere
 *
 * Receptia caracterelor este tratata in ISR. Valoarea primita este
 * reflectata pe P1 ca model pentru cifrele 0..9.
 */
#include <reg51.h>
#include <stdio.h>
#define Led P1
unsigned char c=0;

void serial_init(){
SCON = 0x50; //Modul 1 de lucru pt. portul serial
TMOD = 0x20;
TH1 = 0xFD;
TR1 = 1;
EA = 1;
ES = 1;}

void serial_ISR() interrupt 4{
if (RI == 1){
c = SBUF;
SBUF = c;
RI = 0;}
else if (TI == 1){
TI = 0;}
}

void main(void){ 
P1 = 0x00;
serial_init();
while (1){
switch(c){
case '0': Led =0x3F;break;
case '1': Led =0x06;break;
case '2': Led =0x5B;break;
case '3': Led =0x4F;break;
case '4': Led =0x66;break;
case '5': Led =0x6D;break;
case '6': Led =0x7D;break;
case '7': Led =0x07;break;
case '8': Led =0x7F;break;
case '9': Led =0x6F;break;}
}
}
