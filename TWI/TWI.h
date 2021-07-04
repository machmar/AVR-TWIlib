/*
 * TWI.h
 *
 * Created: 7/4/2021 5:35:32 PM
 * Author: Marek Mach
 * Library based on:
 *	ATmega datasheet (duh)
 *	https://github.com/Sovichea/avr-i2c-library
 */ 


#ifndef TWI_H_
#define TWI_H_

#include <avr/io.h>
#include <util/twi.h>
#include <stdbool.h>

#define __TWI_FREQ_100K 100
#define __TWI_FREQ_250K 250
#define __TWI_FREQ_400K 400

void	TWI_Init(uint16_t TheFrequency);
uint8_t	TWI_Transmit(uint8_t TheSlaveAddress, uint8_t TheData[], uint8_t TheDataSize, bool TheRepeatedStart);
uint8_t	TWI_Recieve(uint8_t TheSlaveAddress, uint8_t TheData[], uint8_t TheDataSize);
void	TWI_Init(uint16_t TheFrequency);
#endif /* TWI_H_ */