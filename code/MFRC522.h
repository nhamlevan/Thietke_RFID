//#include <stdio.h>
#include "spi.h"

//
#define MAX_LEN 16

//
#define PICC_REQIDL 0x26
#define PICC_ANTICOLL 0x93
#define PCD_CALCCRC 0x03
#define PICC_HALT 0x50

//
#define BitFramingReg 0x0D
#define CommandReg 0x01
#define CommIEnReg 0x02
#define CommIrqReg 0x04
#define FIFOLevelReg 0x0A
#define FIFODataReg 0x09
#define ErrorReg 0x06
#define ControlReg 0x0C
#define DivIrqReg 0x05
#define CRCResultRegL 0x22
#define CRCResultRegM 0x20


//
#define PCD_TRANSCEIVE 0x0C
#define PCD_AUTHENT 0x0E
#define PCD_IDLE 0x00


//
#define MI_OK 0
#define MI_NOTAGERR 1
#define MI_ERR 2

void writeMFRC522(uint8_t addr, uint8_t val){
	NSS_LOW;
	spi_tranfer((addr<<1)&0x7E);
	spi_tranfer(val);
	NSS_HIGH;
}

uint8_t readMFRC522(uint8_t addr){
	NSS_LOW;
	spi_tranfer(((addr<<1)&0x7E)|0x80);
	spi_tranfer(0x00);
	NSS_HIGH;
	
	return SPDR;
}

void clearBitMask(uint8_t reg, uint8_t mask){
	uint8_t tmp;
	tmp=readMFRC522(reg);
	writeMFRC522(reg, tmp & (~mask));
}

void setBitMask(uint8_t reg, uint8_t mask){
	uint8_t tmp;
	tmp=readMFRC522(reg);
	writeMFRC522(reg, tmp|mask);
}

uint8_t MFRC522ToCard(uint8_t cmd, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen){
	uint8_t status=MI_ERR, irqEn=0x00, waitIrq=0x00;
	uint16_t lastBits, i,n;
	
	switch(cmd){
		case PCD_AUTHENT:{
			irqEn=0x12;
			waitIrq=0x10;
			break;
		}
		
		case PCD_TRANSCEIVE:{
			irqEn=0x77;
			waitIrq=0x30;
			break;
		}
		
		default: break;
	}
	
	writeMFRC522(CommIEnReg, irqEn|0x80);	//De solicitud de interrupción
	clearBitMask(CommIrqReg, 0x80);			// Borrar todos los bits de petición de interrupción
	setBitMask(FIFOLevelReg, 0x80);			//FlushBuffer=1, FIFO de inicialización
	
	writeMFRC522(CommandReg, PCD_IDLE);	//NO action;Y cancelar el comando
	
	for(i=0; i<sendLen; i++) writeMFRC522(FIFODataReg, sendData[i]); 
	writeMFRC522(CommandReg, cmd);
	if (cmd == PCD_TRANSCEIVE) setBitMask(BitFramingReg, 0x80);
	
	
	i=2000;
	do{
		n=readMFRC522(CommIrqReg);
		i--;
	}while((i!=0) && !(n&0x01) && !(n&waitIrq));
	clearBitMask(BitFramingReg, 0x80);
	
	if(i){
		if(!(readMFRC522(ErrorReg)&0x1B)){
			status=MI_OK;
			if(n&irqEn&0x01) status=MI_NOTAGERR;	
			if(cmd==PCD_TRANSCEIVE){
				n=readMFRC522(FIFOLevelReg);
				lastBits=readMFRC522(ControlReg)&0x07;
				if(lastBits) *backLen=(n-1)*8+lastBits;
				else *backLen=n*8;
				
				if(n==0) n=1;
				if(n>MAX_LEN) n=MAX_LEN;
				
				for(i=0;i<n;i++) backData[i]=readMFRC522(FIFODataReg);
			}
		}
	}
	else status=MI_ERR;
	return status;
}

uint8_t MFRC522Request(uint8_t regMod, uint8_t *TagType){
	uint8_t status, backBits;
	
	writeMFRC522(BitFramingReg, 0x07);		//TxLastBists = BitFramingReg[2..0]	???
	TagType[0]=regMod;
	status = MFRC522ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);
	
	if((status!=MI_OK)||(backBits!=0x10)) status=MI_ERR;
	return status;
}

uint8_t isCard(){
	uint8_t status, str[MAX_LEN];
	status=MFRC522Request(PICC_REQIDL, str);
	if(status==MI_OK) return 1;//false;
	else return 0;
}

uint8_t anticoll(uint8_t *serNum){
	uint8_t status, i, unLen;
	uint8_t serNumCheck=0;
	writeMFRC522(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]
	serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = MFRC522ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);
	
	if (status == MI_OK)
	{
		//?????? Compruebe el número de serie de la tarjeta
		for (i=0; i<4; i++)serNumCheck ^= serNum[i];
	
		if (serNumCheck != serNum[i])  status = MI_ERR;    
    }

    //SetBitMask(CollReg, 0x80);		//ValuesAfterColl=1

    return status;
}

uint8_t readCardSerial(uint8_t *serNum){
	uint8_t status, str[MAX_LEN];
	status=anticoll(str);
	memcpy(serNum, str, 5);//sizeof(5));
	if(status==MI_OK) return 1;
	else return 0;
}

void calculateCRC(uint8_t *pIndata, uint8_t len, uint8_t *pOutData){
	uint8_t i,n;
	clearBitMask(DivIrqReg, 0x04);			//CRCIrq = 0
    setBitMask(FIFOLevelReg, 0x80);			//Claro puntero FIFO
	for (i=0; i<len; i++) writeMFRC522(FIFODataReg, *(pIndata+i));   
    writeMFRC522(CommandReg, PCD_CALCCRC);

	// Esperar a la finalización de cálculo del CRC
    i = 0xFF;
	do 
    {
        n = readMFRC522(DivIrqReg);
        i--;
    }while ((i!=0) && !(n&0x04));			//CRCIrq = 1
	
	//Lea el cálculo de CRC
    pOutData[0] = readMFRC522(CRCResultRegL);
    pOutData[1] = readMFRC522(CRCResultRegM);
}

void halt(){
	uint8_t status, unLen, buff[4];
	buff[0]=PICC_HALT;
	buff[1]=0;
	calculateCRC(buff, 2, &buff[2]);
 
    status = MFRC522ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}
