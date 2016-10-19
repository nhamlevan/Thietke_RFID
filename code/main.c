#include <avr/io.h>

#include "lcd_nokia5110.h"
//#include "spi.h"
#include "MFRC522.h"
//#define as PORTB.2

int main(void){
	//DDRB=0xff;
	//ASF;
	//as=1;
	
	lcd_init();
	lcd_putChar('A');
	//lcd_move(2,5);
	//lcd_cmd(0xce);
	//_delay_ms(100);
	lcd_putChar('X');
	//lcd_move(1,3);
	char s[]="nham la ai?";
	lcd_putString(s);
	//lcd_move(1,1);
	//lcd_putChar('K');
	while(1);
	return 0;
}
