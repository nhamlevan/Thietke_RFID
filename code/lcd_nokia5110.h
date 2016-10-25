
#include <util/delay.h>
#include "font6x8.h"


#define CE PD0
#define RST PD1
#define DC PD2
#define DIN PD3
#define CLK PD4
#define PORT_LCD PORTD
#define DDR_LCD DDRD

//#define sbi(port,bit) port|=_BV(bit)
//#define cbi(port,bit) port&=~_BV(bit)

#define CE_HIGH PORT_LCD|=_BV(CE)
#define RST_HIGH PORT_LCD|=_BV(RST)
#define DC_HIGH PORT_LCD|=_BV(DC)
#define DIN_HIGH PORT_LCD|=_BV(DIN)
#define CLK_HIGH PORT_LCD|=_BV(CLK)

#define CE_LOW PORT_LCD&=~_BV(CE)
#define RST_LOW PORT_LCD&=~_BV(RST)
#define DC_LOW PORT_LCD&=~_BV(DC)
#define DIN_LOW PORT_LCD&=~_BV(DIN)
#define CLK_LOW PORT_LCD&=~_BV(CLK)

#define size_X 84
#define size_Y 6

void lcd_Write(uint8_t data){

	for(uint8_t i=0;i<8;i++){
		if(data&(0x80>>i)) DIN_HIGH;
		else DIN_LOW;
		
		CLK_HIGH;
		CLK_LOW;
		
	}
	_delay_us(400);
}

void lcd_cmd(uint8_t cmd){
  	//_delay_ms(10);
	DC_LOW;
	lcd_Write(cmd);
}


//#define lcd_cmd(cmd) {DC_LOW; lcd_Write(cmd); }
//#define lcd_gotoXY(y,x) {CE_LOW; lcd_cmd(0x40|(y-1)); lcd_cmd(0x80|(x-1)*0x06);}
void lcd_gotoXY(uint8_t y,uint8_t x){
	//_delay_ms(10);
	CE_LOW;
	lcd_cmd(0x40|(y-1));
	lcd_cmd(0x80|(x-1)*0x06);
}

void lcd_putChar(uint8_t chr){
	uint8_t b=chr-32;
	//uint8_t line;
	DC_HIGH;

	lcd_Write(~ASCII[b][0]);
	lcd_Write(~ASCII[b][1]);
	lcd_Write(~ASCII[b][2]);
	lcd_Write(~ASCII[b][3]);
	lcd_Write(~ASCII[b][4]);
	lcd_Write(~ASCII[b][5]);
	/*for(line=0;line<6;line++){
		//CE_HIGH;
		lcd_Write(~ASCII[b][line]);
	}*/
}

void lcd_clear(){
	uint8_t x,y;
	CE_LOW;
	DC_HIGH;
	for(y=0;y<size_Y;y++){
		for(x=0;x<size_X;x++){
			lcd_Write(0xff);
		}
	}
}

void lcd_putString(uint8_t *s){
	while(*s){
		lcd_putChar(*s);
		s++;
	}
}

void lcd_init(){
	DDR_LCD=0xff;

	CLK_LOW;
	_delay_ms(2);
	RST_LOW;
	_delay_ms(2);
	RST_HIGH;
	_delay_ms(2);
	CE_LOW;
	_delay_ms(2);
	
	lcd_cmd(0x21);
	lcd_cmd(0xC0);
	lcd_cmd(0x20);
	lcd_cmd(0x0D);
	
}


void lcd_putStringSX(uint8_t *s, uint8_t size){
	//while(*s){
		//lcd_putChar(*s);
		//s++;
	//}//
	uint8_t i;
	for(i=0;i<size;i++) lcd_putChar(s[i]);
}
