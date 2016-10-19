
#define PORT_SPI PORTB
#define DDR_SPI DDRB
#define NSS PB0
#define MOSI_PIN PB3
#define MISO_PIN PB4
#define SCK_PIN PB5

#define NSS_LOW PORT_SPI&=~_BV(NSS)
#define NSS_HIGH PORT_SPI|=_BV(NSS)

void spi_init(){
	DDR_SPI |=(1<<SCK_PIN)|(1<<MOSI_PIN);
	PORT_SPI |=(1<<MISO_PIN);

	SPCR =(1<<SPIE)|(1<<SPE)|(1<<MSTR)|(1<<CPHA)|(1<<SPR1)|(1<<SPR0);
	DDR_SPI |=(1<<NSS);
	PORT_SPI |=(1<<NSS);
}

void spi_tranfer(uint8_t data){
	NSS_LOW;
	SPDR=data;
	while(bit_is_clear(SPSR,SPIF));
	NSS_HIGH;
}
