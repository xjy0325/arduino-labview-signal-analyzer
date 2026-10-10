/*
 * IncFile1.h
 *
 * Created: 14-03-2026 20:23:03
 *  Author: marie
 */ 


#ifndef UART0_H_
#define UART0_H_

extern void uart0_Init(unsigned int ubrr);
extern void uart1_Init(unsigned int ubrr);
extern void putchUART0 (unsigned char tx);
extern void putchUART1 (unsigned char tx);
extern void putstrUART0 (char *ptr);
extern void putstrUART1 (unsigned char *ptr);
extern char getchUART0(void);
extern char getchUART1(void);
extern int  getlineUART0(unsigned char *line_Whole);

#endif /* UART0_H_ */