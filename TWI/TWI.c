/*
 * TWI.h
 *
 * Created: 7/4/2021 8:09:59 PM
 * Author: Marek Mach
 * Library based on:
 *	ATmega datasheet (duh)
 *	https://github.com/Sovichea/avr-i2c-library
 */ 

#include "TWI.h"

#define OP_SUCCESSFUL 0	//each function returns this number if it finished without any errors - other functions can than check if all went correctly
#define TW_READ_ACK 1
#define TW_READ_NACK 0	//simple macros so it's easier to read the code

void TWI_Init(uint16_t TheFrequency) {
	PRR &= ~(1 << 7);	//disable power-reduction on two-wire communication
	TWSR &= ~(0b11);	//set TWPS1 and TWPS0 to 00 - prescaler of 1
	TWBR = (F_CPU / (2000*(long)TheFrequency)) - 8;			//calculates the TWBR from F_CPU and TheFrequency (prescaler is always set to 1 for simplicity)
	TWCR = (1 << TWEN);	//enable the TWI hardware
}

uint8_t TWI_Start() {
	TWCR =  (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);	//send START
	
	while (!(TWCR & (1 << TWINT)));		//wait for the TWINT flag
	
	if (TW_STATUS != TW_START && TW_STATUS != TW_REP_START) return TW_STATUS;	//check if there is an error

	return OP_SUCCESSFUL;
}

void TWI_Stop() {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);	//send STOP
}

uint8_t TWI_WriteSLA(uint8_t TheSLA) {
	TWDR = TheSLA;						//put slave address with read/write flag (already in TheSLA) into the TWDR data register
	TWCR = (1 << TWINT) | (1 << TWEN);	//initialize transmission
	
	while (!(TWCR & (1 << TWINT)));	//wait for the TWINT flag
	
	if (TW_STATUS != TW_MT_SLA_ACK && TW_STATUS != TW_MR_SLA_ACK) return TW_STATUS;	//check if there is an error

	return OP_SUCCESSFUL;
}

uint8_t TWI_Write(uint8_t TheData) {
	TWDR = TheData;						//put data to be sent into the TWDR data register
	TWCR = (1 << TWINT) | (1 << TWEN);	//initialize transmission
	
	while (!(TWCR & (1 << TWINT))); //wait for the TWINT flag
	
	if (TW_STATUS != TW_MT_DATA_ACK) return TW_STATUS;	//check if there is an error
	
	return OP_SUCCESSFUL;
}

uint8_t TWI_Read(bool TheACKReply) {
	if (TheACKReply) {	//end with ACK
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);	//initialize transmission with the ACK bit
		while (!(TWCR & (1 << TWINT)));						//wait for the TWINT flag
		if (TW_STATUS != TW_MR_DATA_ACK) return TW_STATUS;	//check if there is an error
	}
	else {	//end with NACK
		TWCR = (1 << TWINT) | (1 << TWEN);					//initialize transmission without the ACK bit
		while (!(TWCR & (1 << TWINT)));						//wait for the TWINT flag
		if (TW_STATUS != TW_MR_DATA_NACK) return TW_STATUS;	//check if there is an error
	}
	
	uint8_t data = TWDR;	//get the received data from the TWDR register
	return data;
}

uint8_t TWI_Transmit(uint8_t TheSlaveAddress, uint8_t TheData[], uint8_t TheDataSize, bool TheRepeatedStart) {
	uint8_t ErrorCode;
	
	ErrorCode = TWI_Start();	//send START
	if (ErrorCode != OP_SUCCESSFUL) return ErrorCode;	//return if there is an error, otherwise carry on
	
	ErrorCode = TWI_WriteSLA(TheSlaveAddress << 1 | TW_WRITE);	//send slave address with the Read/Write flag
	if (ErrorCode != OP_SUCCESSFUL) return ErrorCode;	//return if there is an error, otherwise carry on
	
	for (int i = 0; i < TheDataSize; i++) {	//send all the data
		ErrorCode = TWI_Write(TheData[i]); //send the data
		if (ErrorCode != OP_SUCCESSFUL) return ErrorCode; //return if there is an error, otherwise carry on
	}

	if (!TheRepeatedStart) TWI_Stop();	//don't send STOP if TheRepeatedStart is enabled
	
	return OP_SUCCESSFUL;
}

uint8_t TWI_TransmitByte(uint8_t TheSlaveAddress, uint8_t TheData, bool TheRepeatedStart) {
	uint8_t ErrorCode;
	
	ErrorCode = TWI_Start();	//send START
	if (ErrorCode != OP_SUCCESSFUL) return ErrorCode;	//return if there is an error, otherwise carry on
	
	ErrorCode = TWI_WriteSLA(TheSlaveAddress << 1 | TW_WRITE);	//send slave address with the Read/Write flag
	if (ErrorCode != OP_SUCCESSFUL) return ErrorCode;	//return if there is an error, otherwise carry on
	
	ErrorCode = TWI_Write(TheData); //send the data
	if (ErrorCode != OP_SUCCESSFUL) return ErrorCode; //return if there is an error, otherwise carry on

	if (!TheRepeatedStart) TWI_Stop();	//don't send STOP if TheRepeatedStart is enabled
	
	return OP_SUCCESSFUL;
}

uint8_t TWI_Recieve(uint8_t TheSlaveAddress, uint8_t TheData[], uint8_t TheDataSize) {
	uint8_t ErrorCode;
	
	ErrorCode = TWI_Start();	//send START
	if (ErrorCode != OP_SUCCESSFUL) return ErrorCode;	//return if there is an error, otherwise carry on
	
	ErrorCode = TWI_WriteSLA(TheSlaveAddress << 1 | TW_READ);	//send slave address with the Read/Write flag
	if (ErrorCode != OP_SUCCESSFUL) return ErrorCode;	//return if there is an error, otherwise carry on
	
	for (uint8_t i = 0; i < TheDataSize - 1; i++) {		//read the bytes into the TheData array. Except the last byte - a NACK needs to be sent there to symbolize the array being full
		TheData[i] = TWI_Read(TW_READ_ACK);
	}
	TheData[TheDataSize - 1] = TWI_Read(TW_READ_NACK);	//send a NACK to end the transmission
	
	TWI_Stop();	//send STOP
	
	return OP_SUCCESSFUL;
}

uint8_t TWI_RecieveByte(uint8_t TheSlaveAddress) {
	uint8_t ErrorCode;
	uint8_t RecievedByte;
	
	ErrorCode = TWI_Start();	//send START
	if (ErrorCode != OP_SUCCESSFUL) return 0;	//return if there is an error, otherwise carry on
	
	ErrorCode = TWI_WriteSLA(TheSlaveAddress << 1 | TW_READ);	//send slave address with the Read/Write flag
	if (ErrorCode != OP_SUCCESSFUL) return 0;	//return if there is an error, otherwise carry on
	
	RecievedByte = TWI_Read(TW_READ_NACK);	//send a NACK to end the transmission
	
	TWI_Stop();	//send STOP
	
	return RecievedByte;
}