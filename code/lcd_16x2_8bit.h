#include <util/delay.h>

#define PORT_LCD PORTD
#define DDR_LCD DDRD

#define PORT_CTRL_LCD PORTC
#define DDR_CTRL_LCD DDRC

#define sbi(port,bit) port|=_BV(bit)
#define cbi(port,bit) port&=~_BV(bit)

#define RS PC0
#define RW PC1
#define EN PC2

#define D0 PD0
#define D1 PD1
#define D2 PD2
#define D3 PD3
#define D4 PD4
#define D5 PD5
#define D6 PD6
#define D7 PD7

#define time_ms 15
#define time_us 400

#define RS_HIGH sbi(PORT_CTRL_LCD,RS)
#define RW_HIGH sbi(PORT_CTRL_LCD,RW)
#define EN_HIGH sbi(PORT_CTRL_LCD,EN)

#define RS_LOW cbi(PORT_CTRL_LCD,RS)
#define RW_LOW cbi(PORT_CTRL_LCD,RW)
#define EN_LOW cbi(PORT_CTRL_LCD,EN)

#define lcd_send(data) { PORT_LCD=data; EN_HIGH; _delay_ms(time_ms); EN_LOW;}
#define lcd_putChar(chr) { RS_HIGH,RW_LOW; lcd_send(chr); _delay_us(time_us);}
#define lcd_cmd(cmd) { RS_LOW; RW_LOW; lcd_send(cmd); _delay_us(time_us);}
#define lcd_move(y,x) lcd_cmd(0x80 + 0x40*(y-1) + x-1)
#define lcd_clear lcd_cmd(0x01);


void lcd_putString(uint8_t *s){
	while(*s){
		lcd_putChar(*s);
		s++;
	}
}
void lcd_init(){
	DDR_CTRL_LCD=0xff;
	DDR_LCD=0xff;
	
	RS_LOW;
	RW_LOW;

	lcd_send(0x38);
	_delay_us(time_us);

	lcd_send(0x0f);
	_delay_us(time_us);

	lcd_send(0x01);
	_delay_us(time_us);

	lcd_send(0x06);
	_delay_us(time_us);


}


