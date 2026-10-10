
#include <avr/io.h>
#include <avr/interrupt.h>

void initTimer0(uint8_t TopValue){
	//mode CTC with OCRn as Top Value + prescaler 64 
	TCCR0A |= (1<<WGM01);
	TCCR0B |=(1<<CS00)|(1<<CS01);
	
	//OCR1A holds TopValue
	OCR0A = TopValue;
	
	//enable OCI
	TIMSK0 |= (1<<OCIE0A);
}

void initTimer1CTC(uint16_t TopValue){
	//mode CTC with OCRn as Top Value + prescaler 64
	TCCR1B |= (1<<WGM12);
	TCCR1B |= (1<<CS11)| (1<<CS10);
	
	//OCR1A holds TopValue
	OCR1A = TopValue;
	
	//enable OCI
	TIMSK1 |= (1<<OCIE1A);
}

void initTimer1(uint16_t TopValue){
	//mode PWM phase and frequency correct with ICR as Top Value + prescaler 8 + OCR1A 1500 
	TCCR1B |= (1<<CS11)|(1<<WGM13);
	
	//clear OC1A on compare match
	TCCR1A |= (1<<COM1A1);
	
	//count up from 0 to 20000 and back to 0
	TCNT1 = 0;
	ICR1 = TopValue;
	
	//compare value for duty cycle 1st gang
	OCR1A = 1500;
	
	//set PWM output
	DDRB |= (1<<PB5);
}

void initTimer3(uint16_t TopValue){
	//mode PWM phase and frequency correct with ICR as Top Value + prescaler 8 + OCR3A 1500
	TCCR3B |= (1<<CS31)|(1<<WGM33);
	
	//clear OC3A on compare match
	TCCR3A |= (1<<COM3A1);
	
	//count up from 0 to 20000 and back to 0
	TCNT3 = 0;
	ICR3 = TopValue;
	
	//compare value for duty cycle 1st gang
	OCR3A = 1500;
	
	//set PWM output
	DDRE |= (1<<PE3);
}

void initTimer4(uint16_t TopValue){

    //mode PWM phase and frequency correct with ICR as Top Value + prescaler 8 + OCR3A 1500
    TCCR4B |= (1<<CS41)|(1<<WGM43);

    //clear OC4A on compare match
    TCCR4A |= (1<<COM4A1);

    //count up from 0 to 20000 and back to 0
    TCNT4 = 0;
    ICR4 = TopValue;

    //compare value for duty cycle 1st gang
    OCR4A = 1500;

    //set PWM output
    DDRH |= (1<<PH3);
}
void initTimer5(int TopValue){

    //mode PWM phase and frequency correct with ICR as Top Value + prescaler 8 + OCR3A 1500
    TCCR5B |= (1<<CS51)|(1<<WGM53);

    //clear OC5A on compare match
    TCCR5A |= (1<<COM5A1);

    //count up from 0 to 20000 and back to 0
    TCNT5 = 0;
    ICR5 = TopValue;

    //compare value for duty cycle 1st gang
    OCR5A = 1500;

    //set PWM output
    DDRL |= (1<<PL3);
}
/*example for delay 1s by using corresponding ISR
volatile int Round = 0;
volatile char DelayFlag = 0;
ISR(TIMER1_COMPA_vect){
    Round++;
    if (Round == 1000){
        DelayFlag = 1;
        Round = 0;
    }
}*/