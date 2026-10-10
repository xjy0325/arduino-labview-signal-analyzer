/*
 * Oscillator Mega.c
 *
 * Created: 08-06-2026 09:33:53
 * Author : Xingjia
 */ 
#define F_CPU 16000000UL
#define F_timer 250000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#include "UART.h"
#include "SPI.h"
#include "timer.h"
#include "ADC.h"

#define BAUD 115200
#define MYUBRRF (F_CPU/8/BAUD -1) //double speed

ISR (TIMER1_COMPA_vect){
	
	ADCSRA |= (1<<ADSC);  //start conversion for next time data reading

}

volatile unsigned int RecordLength = 300; //default record length
volatile unsigned int ADCcounter = 0; //record how many values
volatile unsigned int ADCInd = 5; // index of buffer
volatile unsigned char Data_Ready = 0; //buffer ready

volatile unsigned char ADCValue1[1007] = {0}; //buffer 1 for Oscil
volatile unsigned char ADCValue2[1007] = {0}; //buffer 2 for Oscil

volatile unsigned char BPMode = 0; //BodePlotMode
volatile unsigned char ComStart = 0; //start Comparison
volatile unsigned char AmpValues[262] = {0}; //buffer for Amps
volatile unsigned char AmpMax = 0; //max Amp
volatile unsigned char AmpMin = 0; //min Amp
volatile unsigned char MinNow = 0; //current min Amp
volatile unsigned char MaxNow = 0; // current max Amp

ISR (ADC_vect){
      switch (BPMode)
      {
                case 0: // oscil mode
				if (ADCcounter<RecordLength){
					ADCValue1[ADCInd] = ADCH;
					ADCInd++;
					ADCcounter++;
					} else if (ADCcounter == RecordLength){
					ADCValue1[ADCInd] = ADCH;
					Data_Ready = 1;
					ADCcounter++;
					ADCInd = 5;
					} else if (ADCcounter<(2*RecordLength) && ADCcounter>RecordLength){
					ADCValue2[ADCInd] = ADCH;
					ADCInd++;
					ADCcounter++;
					} else if (ADCcounter == (2*RecordLength)){
					ADCValue2[ADCInd] = ADCH;
					Data_Ready = 2;
					ADCcounter = 1;
					ADCInd = 5;
				}
                     break;
                case 1: //BP mode
				     if (ComStart){
                         if (ADCcounter<1000)
                         {
							 MaxNow = ADCH;
							 MinNow = MaxNow;
							 if  (AmpMax < MaxNow) AmpMax = MaxNow;
							 if (AmpMin > MinNow) AmpMin = MinNow;
							 ADCcounter++;
                         }else if (ADCcounter == 1000)
                         {
							 MaxNow = ADCH;
							 MinNow = MaxNow;
							 if  (AmpMax < MaxNow) AmpMax = MaxNow;
							 if (AmpMin > MinNow) AmpMin = MinNow;
                         }
					 }
                     break;
          }//end switch        
}

volatile char ReceiveFlag = 0;
volatile int ind = 0;
volatile int Lab_length = 0;
volatile unsigned char GetLab[11] = {0};
ISR(USART1_RX_vect){
	GetLab[ind] = UDR1;
	ind++;
	
	if (ind == 5){
		switch(GetLab[4]){
			case 0x01: Lab_length = 9;  break;
			case 0x02: Lab_length = 11; break;
			case 0x03: Lab_length = 7;  break;
		}
	}
	
	if (Lab_length > 0 && ind >= Lab_length){
		ReceiveFlag = 1;
	}
}
		
//send data to labview
void PutStrlab(volatile unsigned char *ptr, unsigned int length){
	for(int i = 0; i < length; i++){
		putchUART1(*ptr);
		ptr++;
	}
}

//send data to PC terminal for testing
void PutStrPC(volatile unsigned char *ptr, unsigned int length){
	for (int i=0; i<length; i++){
		putchUART0(*ptr);
		ptr++;
	}
}

//error counter
unsigned int ErrorCounter = 0;

//send data to FPGA
unsigned char PutStrFPGA(unsigned char *fpga) {
	PORTB &=~(1<<PB0); //wake the slave!
    _delay_us(1);

    for(int i = 0; i < 5; i++){
	    SPDR = fpga[i];
	    while(!(SPSR &(1<<SPIF))) ;
    }
	PORTB |=(1<<PB0);
	//let the slave sleep
	unsigned char ABReturn = SPDR;
	if (!(ABReturn == 0x00)) ErrorCounter++;
	PutStrPC(fpga,5); //control what I sent to FPGA
	putchUART0(ABReturn);
	putchUART0(ErrorCounter);
	return;
}

//from claude prompt:transfer digital number to frequency according to the formula in the pic
uint32_t DigitalToFrequency(uint8_t FDigital){
	
	uint8_t f7 = (FDigital >> 7) & 1;
	uint8_t f6 = (FDigital >> 6) & 1;
	uint8_t f5 = (FDigital >> 5) & 1;
	uint8_t f4 = (FDigital >> 4) & 1;
	uint8_t f3 = (FDigital >> 3) & 1;
	uint8_t f2 = (FDigital >> 2) & 1;
	uint8_t f1 = (FDigital >> 1) & 1;
	uint8_t f0 = (FDigital >> 0) & 1;

	uint16_t FNew = (f7 << 9) | (f6 << 8) | (f5 << 7) | (f4 << 6) |
	(f3 << 4) | (f2 << 3) |
	(f1 << 1) | (f0 << 0);

	return (uint32_t)FNew * 24;
}

int main(void)
{
    uart0_Init(MYUBRRF); //communication with PC for test
	uart1_Init(MYUBRRF); //communication with Labview
    spiinit(); //send message to FPGA
    initTimer1CTC(400); //default start from 5000 sample rate
    initADC(); //receive FPGA data

    sei();

            //data protocol for oscil 
            ADCValue1[0] = 0x55;
            ADCValue1[1] = 0xaa;
            ADCValue1[4] = 0x02;
            ADCValue2[0] = 0x55;
            ADCValue2[1] = 0xaa;
            ADCValue2[4] = 0x02;
            ADCValue1[3] = 0x33;
            ADCValue1[2] = 0x01;
            ADCValue2[3] = 0x33;
            ADCValue2[2] = 0x01;
            ADCValue1[305] = 0x00;
            ADCValue1[306] = 0x00;
            ADCValue2[305] = 0x00;
            ADCValue2[306] = 0x00;
	unsigned char UpdateGenPan[11] = {0}; //data protocol for labgenpan
            UpdateGenPan[0] = 0x55;
            UpdateGenPan[1] = 0xaa;
            UpdateGenPan[2] = 0x00;
            UpdateGenPan[3] = 0x0b;
            UpdateGenPan[4] = 0x01;
            UpdateGenPan[9] = 0x00;
            UpdateGenPan[10] = 0x00;
    unsigned char UpdateGenSig[5] = {0}; //data protocol for FPGA
            UpdateGenSig[0] = 0xFF;
            UpdateGenSig[3] = 0x00;
			UpdateGenSig[4] = 0xAB;//Ack Byte
			
			//data protocol for bodeplot
			AmpValues[0] = 0x55;
			AmpValues[1] = 0xaa;
			AmpValues[2] = 0x01;
			AmpValues[3] = 0x06;
			AmpValues[4] = 0x03;
			AmpValues[5] = 0xFF;
			AmpValues[6] = 0xFF;
			AmpValues[260] = 0x00;
			AmpValues[261] = 0x00;
			
	
	//ACK return
	
		
	//test information
	char *Info1="\nOCR1A Value: \n";
	char buf1[10];
	char *Info2="\nRecordLength Value: \n";
	char buf2[10];
    
    //run or stop of signal generating
    char RunorStop = 0;
	 
	//calculate minimum record length
	unsigned int NewSampleRate = 0;
	unsigned int MinimumRecordLen = 0;
	

	
	//test SPI med 10000 packages with 10 wrong packages
	   unsigned char testpackage[5] = {0};
	    testpackage[0] = 0xff;
	    testpackage[1] = 0x04;
	    testpackage[3] = 0x00;
	    testpackage[4] = 0xab;
	    PutStrFPGA(testpackage); //reset
		testpackage[0] = 0xff;
		testpackage[1] = 0x01;
		testpackage[2] = 0xcc;
		testpackage[3] = 0x00;
		testpackage[4] = 0xab;
	
	for (unsigned int i = 0; i < 5000; i++){
		PutStrFPGA(testpackage);
	}//send 5000 right packages
	testpackage[3] = 0xcc;
	for (unsigned int i = 0; i < 10; i++){
		PutStrFPGA(testpackage);
	}//send 10 incorrect packages
	testpackage[3] = 0x00;
	for (unsigned int i = 0; i < 4990; i++){
		PutStrFPGA(testpackage);
	}
	

     while (1) 
    {

        if (ReceiveFlag){
	    //clear flag
	    ReceiveFlag = 0;
	    ind = 0;
        Lab_length = 0;
	    
		      switch (GetLab[4])
		    {
		             case 0x01: //update generator panel accordingly
		                     switch (GetLab[5]){
			                        case 0x00: // enter data
			                                  switch (UpdateGenPan[5])
			                                      {
				                                  case 0x00: //shape
				                                  UpdateGenPan[6] = GetLab[6];
				                                  UpdateGenSig[1] = 0x01; //address byte
				                                  UpdateGenSig[2] = GetLab[6]+1;
			                                 	    break;
			                                	  case 0x01: //amp
				                                  UpdateGenPan[7] = GetLab[6];
				                                  UpdateGenSig[1] = 0x02; //address byte
				                                  UpdateGenSig[2] = GetLab[6];
				                                    break;
				                                 case 0x02: //freq
				                                 UpdateGenPan[8] = GetLab[6];
			                             	     UpdateGenSig[1] = 0x03; //address byte
				                                 UpdateGenSig[2] = GetLab[6];
				                                    break;
			                                      }
											 PutStrlab(UpdateGenPan, 11); // send to panel
											 PutStrFPGA(UpdateGenSig); // send to FPGA with AB byte
			                                 break; //BTN0 end
			                        case 0x01: //select: update indicator light
			                                 if (UpdateGenPan[5] < 0x02) UpdateGenPan[5]++;
			                                 else UpdateGenPan[5] = 0x00;
											 PutStrlab(UpdateGenPan, 11); // send to panel
			                                 break;
			                        case 0x02: //run or stop
			                                 RunorStop = !RunorStop;
			                                 if (RunorStop) UpdateGenSig[1] = 0x05; //address byte
			                                 else UpdateGenSig[1] = 0x06; //address byte
			                                 PutStrFPGA(UpdateGenSig); // send to FPGA with AB byte
			                                 break;
			                         case 0x03: //reset
			                                 UpdateGenPan[5]=0x00;
			                                 UpdateGenPan[6]=0x00;
			                                 UpdateGenPan[7]=0x00;
			                                 UpdateGenPan[8]=0x00;
			                                 UpdateGenSig[1] = 0x09; //address byte
			                                 PutStrFPGA(UpdateGenSig);// send to FPGA with AB byte
											 PutStrlab(UpdateGenPan,11);// send to panel
			                                 break;
		                                       }
			           break; //Panel End
		           case 0x02: //update ADC parameters accordingly
		                    //update Sample Rate
		                    NewSampleRate = ((uint16_t)GetLab[5] << 8) | GetLab[6];
		    
		                    OCR1A = (F_timer / NewSampleRate)-1;//update sample rate
		                    //check OCR1A value with PC terminal
		                    sprintf(buf1, "%d", OCR1A);
		                    putstrUART0(Info1);
		                    putstrUART0(buf1);
		    
		                    //update and control Record Length
		                    RecordLength = (GetLab[7] << 8) | GetLab[8];
							//get from Claude. Prompt: how to round up the result instead of truncation
		                    MinimumRecordLen = (70UL * NewSampleRate + (115200UL - 10UL * NewSampleRate) - 1)
		                    / (115200UL - 10UL * NewSampleRate);
		                    if (RecordLength < MinimumRecordLen) RecordLength = MinimumRecordLen;
		                    //check RecordLength value with PC terminal
		                    sprintf(buf2, "%d", RecordLength);
		                    putstrUART0(Info2);
		                    putstrUART0(buf2);
		    
		                    ADCValue1[3] = (RecordLength+7);
		                    ADCValue1[2] = (RecordLength+7)>>8;
		                    ADCValue2[3] = (RecordLength+7);
		                    ADCValue2[2] = (RecordLength+7)>>8;
		                    ADCValue1[RecordLength+5] = 0x00;
		                    ADCValue1[RecordLength+6] = 0x00;
		                    ADCValue2[RecordLength+5] = 0x00;
	                 	    ADCValue2[RecordLength+6] = 0x00;
		                    ADCcounter = 0;
		                    ADCInd = 5;
			           break;
		        case 0x03: //Bode Plot start
		             // start BPmode
		             BPMode = 1;
		             ADCInd = 6; //op t0 259
					
		    
		             // sin + 255 AMp
		             UpdateGenSig[1] = 0x01;
		             UpdateGenSig[2] = 0x04;
		             PutStrFPGA(UpdateGenSig);
		             UpdateGenSig[1] = 0x02;
		             UpdateGenSig[2] = 0xFF;
		             PutStrFPGA(UpdateGenSig);
		             UpdateGenSig[1] = 0x03;
			            break;
		    }
         }//receive_data end
        
		// SigFre increment under BPmode
		if (BPMode){
			if (ADCcounter == 1000)
			{
				if (ADCInd < 260) {
					            
					AmpValues[ADCInd] = AmpMax-AmpMin;
					ADCInd++;
					
				}
				ComStart = 0;
				ADCcounter = 0;
			}

			if (ADCInd < 260) {
				if (!ComStart)
				{
					if (DigitalToFrequency(ADCInd-6)>2000)
					{
						OCR1A = 25;
					}else (OCR1A = F_timer / (10 * DigitalToFrequency(ADCInd-3)));
					
					UpdateGenSig[2] = (ADCInd - 6);
					
					PutStrFPGA(UpdateGenSig);

					AmpMax = 0;
					AmpMin = 0;
					ComStart = 1;
				}
				} else {                            
				BPMode = 0;
				ComStart = 0;
				ADCcounter = 0;
				ADCInd = 5;
				PutStrlab(AmpValues,262);
				PutStrPC(AmpValues,262);
				OCR1A = 200;
			}
		}	// end bodeplot 
		
         //when ADCValue collection is ready
		 switch (Data_Ready)
		 {
		 case 1:
			      Data_Ready = 0;
			      ADCValue1[RecordLength+5] = 0x00;
			      ADCValue1[RecordLength+6] = 0x00;
			      PutStrlab(ADCValue1, (RecordLength+7));
		 	break;
		 case 2:
		          Data_Ready = 0;
				  ADCValue2[RecordLength+5] = 0x00;
		          ADCValue2[RecordLength+6] = 0x00;
		          PutStrlab(ADCValue2, (RecordLength+7));
		    break;
		 }//end data ready switch
       }//while loop end
}

