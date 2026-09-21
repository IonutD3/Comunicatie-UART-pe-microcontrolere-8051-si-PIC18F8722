/*
 * 01 - UART asynchronous, polling
 *
 * Virtual Terminal -> PIC18F8722 -> LED-uri pe RD0/RD1.
 * Comenzile sunt 1, 2 si 0.
 */
#include <xc.h>
#include <string.h>
unsigned char c;

void init_USART(){
CSRC = 1;
SYNC = 0;
BRGH = 1;
BAUDCONbits.BRG16 = 0;
SPBRG1 = 12; //19200 bps
TXEN = 1;
CREN = 1;
TRISCbits.RC6 = 0;
TRISCbits.RC7 = 1;
SPEN = 1;}

void write_serial(char c){
while(!TX1IF);
TXREG = c;}

void read_serial(){
while(!RC1IF);
c = RCREG1;}

void write_ms(char ch[]){
for(int i=0;i<=strlen(ch);i++){
write_serial(ch[i]);}
}

void led(){
read_serial();
switch(c){
case '1': RD0 = 1; write_serial(0x0d);write_ms("Led 1 aprins");write_serial(0x0d);break;
case '2': RD1 = 1; write_serial(0x0d);write_ms("Led 2 aprins");write_serial(0x0d);break;
case '0': RD0 = 0; RD1 = 0; write_serial(0x0d);write_ms("Led-uri stinse");write_serial(0x0d);break;}
}

void main(void){
TRISD = 0x0;
LATD = 0x0;
init_USART();
write_ms("Apasati 1, 2 sau 0");write_serial(0x0d);
while (1){
led();}
}
