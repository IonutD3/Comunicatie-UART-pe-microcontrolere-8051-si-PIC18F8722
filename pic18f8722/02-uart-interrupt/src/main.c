/*
 * 02 - UART asynchronous cu intrerupere RX
 *
 * Comenzile 1, 2 si 0 controleaza LED-urile RD0/RD1.
 */
#define XTAL_FREQ 4000000
#include "pic18f8722.h"
#include <string.h>
unsigned char c;

void init_USART(){
//transmisie
CSRC = 1;
BRGH = 1; 
SYNC = 0;
TXEN = 1;
SPEN = 1;
//baud_rate
BAUDCON1bits.BRG16 = 0;
SPBRG1 = 25;
//receptie
CREN = 1;
//activare
GIE = 1;
PEIE = 1;
RC1IE = 1;
TX1IE = 1;
TRISCbits.RC6 = 0;
TRISCbits.RC7 = 1;
SPEN = 1;}

void read_serial(){
while(RC1IF == 0);
c = RCREG1;}

void write_serial(char c){
while(TX1IF == 0);
TXREG = c;}

void write_mes(char cv[]){
for(int i=0;i<=strlen(cv);i++){
write_serial(cv[i]);}
}

void interrupt ISR(){
if((TX1IF)){
TX1IE = 0;}
if((RC1IF)){
read_serial();
switch(c){
case '1': RD0 =1;write_serial(0x0d);break;
case '2': RD1 =1;write_serial(0x0d);break;
case '0': RD0 =0; RD1 = 0;write_serial(0x0d);break;}
RC1IF = 0;}
}

void main(void){
TRISD = 0x0;
LATD = 0x0;
init_USART();
write_mes("Apasati 1 sau 2 pentru aprindere led-uri, respectiv 0 pentru stingere:");
write_serial(0x0d);
while(1) 
{} // Bucla principala este libera pentru alte activitati.
}
