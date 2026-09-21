/*
 * 01 - UART 8051 in polling
 *
 * Terminalul trimite 1, 2 sau 0 pentru a controla LED-urile de pe P1.
 */
#include <reg51.h>
#include <stdio.h>
sbit led1 = P1^0;
sbit led2 = P1^1;
unsigned char c;

void serial_init(){
SCON = 0x50; //Modul 1 de lucru pt. portul serial
TMOD = 0x20;
TH1 = 0xFD;
TR1 = 1;}

void write_ch(unsigned char c){
SBUF = c;
while(TI==0);
TI = 0;}

void write_msg(unsigned char *c){ 
while(*c != 0){
write_ch(*c++);}
}

void init_term(){
write_ch(0x0d);
write_ch(0x0a);
write_ch(0x0);}

void read_serial(void){
while(RI ==0);
c = SBUF;
write_ch(c);
RI = 0;}

void led_on_off(){
read_serial();
switch(c){
case '1': led1 =1;init_term();break;
case '2': led2 =1;init_term();break;
case '0': led1 =0; led2 = 0; init_term();break;}
}

void main(void){ 
P1 = 0x00;
serial_init();
write_msg("Apasati 1 sau 2 pentru aprindere led-uri, respectiv 0 pentru initializare:");
init_term();
while (1){
led_on_off();}
}
