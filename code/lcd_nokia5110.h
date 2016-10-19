#include <util/delay.h>
#include "font6x8.h"


#define CE PD0
#define RST PD1
#define DC PD2
#define DIN PD3
#define CLK PD4
#define PORT_LCD PORTD
#define DDR_LCD DDRD

#define sbi(port,bit) port|=_BV(bit)
#define cbi(port,bit) port&=~_BV(bit)

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
#define size_Y 48

void lcd_Write(uint8_t data){
	//uint8_t i;
	//DC_LOW;
	for(uint8_t i=0;i<8;i++){
		if(data&(0x80>>i)) DIN_HIGH;
		else DIN_LOW;
		
		CLK_HIGH;
		_delay_ms(100);
		CLK_LOW;
	}
}

void lcd_cmd(uint8_t cmd){
	DC_LOW;
	lcd_Write(cmd);
}

void lcd_gotoXY(uint8_t y,uint8_t x){
	CE_LOW;
	lcd_cmd(0x40|y);
	lcd_cmd(0x80|x);
}

void lcd_putChar(uint8_t chr){
	uint8_t b=chr-32;
	uint8_t line;
	DC_HIGH;
	CE_HIGH;
	for(line=0;line<5;line++){
		//CE_HIGH;
		lcd_Write(ASCII[b][line]);
	}
}

void lcd_clear(){
	uint8_t x,y;
	for(y=0;y<size_Y;y++){
		for(x=0;x<size_X;x++){
			lcd_putChar(0x00);
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
	_delay_us(100);
	RST_LOW;
	_delay_us(100);
	RST_HIGH;
	_delay_us(100);
	CE_LOW;
	_delay_us(100);
	
	lcd_cmd(0x21);
	lcd_cmd(0xB1);
	lcd_cmd(0xC0);
	lcd_cmd(0x04);
	lcd_cmd(0x14);
	lcd_cmd(0x20);
	lcd_cmd(0x0D);
	
}
