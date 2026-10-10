#include <avr/io.h>

void uart0_Init(unsigned int ubrr){
	UCSR0A |= (1<<U2X0); //double speed
	
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0); //duplex and interrupt enable
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00); //8 bit with 1 stop bit and no parity
	
	UBRR0H = (unsigned char) (ubrr>>8);
	UBRR0L = (unsigned char) ubrr;
}

void uart1_Init(unsigned int ubrr){
	UCSR1A |= (1<<U2X1); //double speed
	
	UCSR1B |= (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1); //duplex and interrupt enable
	UCSR1C |= (1<<UCSZ11)|(1<<UCSZ10); //8 bit with 1 stop bit and no parity
	
	UBRR1H = (unsigned char) (ubrr>>8);
	UBRR1L = (unsigned char) ubrr;
}

void putchUART0 (unsigned char tx){
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = tx;
}

void putchUART1 (unsigned char tx){
	while (!(UCSR1A & (1<<UDRE1)));
	UDR1 = tx;
}

void putstrUART0 (unsigned char *ptr){
	while (*ptr){
		putchUART0(*ptr);
		ptr++;
	}
}

void putstrUART1 (unsigned char *ptr){
	while (*ptr){
		putchUART1(*ptr);
		ptr++;
	}
}

char getchUART0(void){
	while (!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

char getchUART1(void){
	while (!(UCSR1A & (1<<RXC1)));
	return UDR1;
}

int getlineUART0(unsigned char *line_Whole){
	int counter = 0;
	char temp;
	while(((temp=getchUART0())!='\n')&&(temp!='\r')){
		*line_Whole = temp;
		counter++;
		line_Whole++;
	}
	*line_Whole = temp;
	counter++;
	if (((temp=getchUART0())=='\n')||(temp=='\r')){
		line_Whole++;
		*line_Whole = temp;
		counter++;
	}
	line_Whole++;
	*line_Whole='\0';
	return counter;
}
