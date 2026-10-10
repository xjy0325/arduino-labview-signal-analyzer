/*
 * IncFile1.h
 *
 * Created: 28-02-2026 00:26:15
 *  Author: marie
 */ 


#ifndef SPI_H
#define SPI_H

extern void spiinit(void);
extern unsigned char putcSPI_master(unsigned char cx);
extern unsigned char getcSPI_master(unsigned char data);
extern void putcSPI_slave(unsigned char cx);
unsigned char getcSPI_slave(void);

#endif