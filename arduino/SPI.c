/*
 * CFile1.c
 *
 * Created: 28-02-2026 00:26:51
 *  Author: marie
 */
#include <avr/io.h>

void spiinit(){
	DDRB |=(1<<DDB2)|(1<< DDB1)|(1<<DDB0);//open MOSI SCK SS as output pin
	SPCR |=(1<<SPE)|(1<<MSTR);
	SPSR |=(1<<SPI2X);
	SPCR |=(1<<SPR1);//prescaller 32 to 500KHZ
}

unsigned char putcSPI_master(unsigned char cx) {
	PORTB &=~(1<<PB0);  //wake the slave!
	SPDR = cx;
	while(!(SPSR &(1<<SPIF))) ;
	PORTB |=(1<<PB0);
	// wait for data to be shifted serial out
	//let the slave sleep
	return SPDR;
}

unsigned char getcSPI_master(unsigned char data)
{
	SPDR=data;
	while(!(SPSR &(1<<SPIF))) ; // wait for a byte to be shifted in –> SPIF=1
	return SPDR;
	// return data in SPDR register also clears the SPIF flag
}

void putcSPI_slave(unsigned char cx){
		unsigned char tmp;
		SPDR = cx;
		while(!(SPSR & (1 << SPIF)));
		tmp = SPDR;
		// wait for data byte to be shifted out
		// clear the SPIF flag
}

unsigned char getcSPI_slave(void){
	while(!(SPSR & (1 << SPIF))); // wait for data byte to be shifted in
	return SPDR; // also clear the SPIF flag
}

