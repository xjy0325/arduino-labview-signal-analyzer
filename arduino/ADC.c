/*
 * initADC.c
 *
 * Created: 29-04-2026 18:46:12
 *  Author: marie
 */ 
#include <avr/io.h>

void initADC(){
	
	ADCSRA |= (1<<ADEN)|(1<<ADPS2) ;//enable ADC with 16 prescaler => clk = 1M Hz
	
	ADCSRA |= (1<<ADIE); //enable interrupt

    ADMUX |= (1<<ADLAR); // 8 bit
	
	ADCSRA |= (1<<ADSC);//start first time sample
 
    
	
}