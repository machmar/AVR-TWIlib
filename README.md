# AVR-TWIlib
This library makes it easy to use the Two Wire Interface of the AVR ATmega MCUs.  
Curently only supports the MCU to be in master mode (slave mode will be added in the future).  
This library has been tested on the ATmega 328p.  
Thanks to the simple math, this library works with non-standard SCL frequencies and non-standard CPU clock frequencies.

## Usage:
* Initialize the TWI with `TWI_Init(SCLFrequency)`.  
* Use the `TWI_Transmit(SlaveAddress, Data, DataLenght, RepeatedStart)` function to send data to a slave device.  
* * Use the `TWI_TransmitByte(slaveAddress, Data, RepeatedStart)`function to send a single byte to a slave device.  
* Use the `TWI_Transmit(SlaveAddress, Data, DataLenght)` function to receive data from a slave device.  
* * Use the `TWI_ReceiveByte(slaveAddress)`function to send a single byte to a slave device (this function returns the received data).  
    
Macros:  

For *SCL Frequency* selection  

    __TWI_FREQ_100K 100  
    __TWI_FREQ_250K 250  
    __TWI_FREQ_400K 400  
