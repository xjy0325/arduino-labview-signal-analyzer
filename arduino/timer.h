/*
 * timer.h
 *
 * Created: 06-04-2026 14:01:49
 *  Author: marie
 */ 


#ifndef TIMER_H_
#define TIMER_H_

extern void initTimer5(uint16_t TopValue);
extern void initTimer4(uint16_t TopValue);
extern void initTimer3(uint16_t TopValue);
extern void initTimer1CTC(uint16_t TopValue);
extern void initTimer1(uint16_t TopValue);
extern void initTimer0(uint8_t TopValue);

#endif /* TIMER_H_ */