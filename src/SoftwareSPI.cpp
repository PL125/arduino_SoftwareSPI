/* Serial Bit-bang Interface (C) 2018 Rev Phil Morris */

#include <SoftwareSPI.h>

SSPIClass SSPI;

uint8_t SSPIClass::begin(uint8_t _sck, int8_t _miso, int8_t _mosi) {
	// start SBI on the specified pins with SSPI_MODE0 and MSBFIRST as default
	// check for valid digital pins
	if(_sck > (NUM_DIGITAL_PINS - 1)) return 0x01;	// invalid SCK pin
	else if(_miso > (NUM_DIGITAL_PINS - 1)) return 0x02;	// invalid MISO pin
	else if(_mosi > (NUM_DIGITAL_PINS - 1)) return 0x03;	// invalid MOSI pin
	else if(_miso == NOT_A_PIN && _mosi == NOT_A_PIN) return 0x04;	// no MISO or MOSI pin specified	
	// set the working variables
	sckPin = _sck;
	mosiPin = _mosi;
	transferMode = SSPI_READ_WRITE;
	dataMode = SSPI_MODE0 >> 2;	// make SSPI_MODEx more usable
	bitOrder = 0x07;	// MSBFIRST
	if(_miso == NOT_A_PIN) transferMode = SSPI_WRITE_ONLY;
	else if(_mosi == NOT_A_PIN) transferMode = SSPI_READ_ONLY;
	// configure the SCK pin
	makeRegMask(_sck,&sckReg,&sckBit,OUTPUT);
	pinMode(_sck,OUTPUT);
	// if SSPI_MODE = 2 or 3 set SCK start level HIGH, else LOW
	dataMode >> 1 ? *sckReg |= sckBit : *sckReg &= ~sckBit;
	// configure the MISO pin if specified
	if(transferMode != SSPI_WRITE_ONLY) {
		makeRegMask(_miso,&misoReg,&misoBit,INPUT);
		pinMode(_miso, INPUT);
	}
	// configure the MOSI pin if specified
	if(transferMode != SSPI_READ_ONLY) {
		makeRegMask(_mosi,&mosiReg,&mosiBit,OUTPUT);
		pinMode(_mosi, OUTPUT);
		*mosiReg &= ~mosiBit;	// MOSI LOW
	}
	return false;
}

void SSPIClass::beginTransaction(uint8_t _dataMode, uint8_t _dataOrder) {
	dataMode = _dataMode >> 0x02;
	bitOrder = _dataOrder * 0x07;
}

uint8_t SSPIClass::beginTransaction(uint8_t _sck, int8_t _miso, int8_t _mosi, uint8_t _dataMode, uint8_t _dataOrder) {
	beginTransaction(_dataMode, _dataOrder);
	return begin(_sck, _miso, _mosi);
}

void SSPIClass::endTransaction(void) {
	// reset the SCK and MOSI pins to INPUTS
	pinMode(sckPin, INPUT);
	if(mosiPin >= 0) pinMode(mosiPin, INPUT);
}

	
uint8_t SSPIClass::transfer(uint8_t outData) {
	uint8_t inData = 0;
	
	for (uint8_t x = 0;x<8;x++) {
		uint8_t oldSREG = SREG;
		// set the MOSI pin to data bit(x) value if SSPI_READ_ONLY is NOT set. (x ^ bitOrder) is used to invert
		// the outData bit selection if SSPI_MSBFIRST is selected else, no inversion
		if(transferMode != SSPI_READ_ONLY) (outData >> (x ^ bitOrder)) & 0x01 ? *mosiReg |= mosiBit : *mosiReg &= ~mosiBit;
		
		// start the SCK pin pulse, H > L : L > H
		dataMode >> 1 ? *sckReg &= ~sckBit : *sckReg |= sckBit;	// CHPA = 1/0
		
		// read the MISO pin and set the inData bit(x) accordingly if "read at Leading edge" (CPOL = 0)
		// again, (x ^ bitOrder) inverts the bit selection if MSBFIRST is specified
		if (transferMode != SSPI_WRITE_ONLY && !(dataMode & 0x01) && (*misoReg & misoBit)) {
			delayMicroseconds(SSPI_SETTLE_TIME);
			inData |= (1 << (x ^ bitOrder));
		}
		
		// end the SCK pin pulse, L > H : H > L
		dataMode >> 1 ? *sckReg |= sckBit : *sckReg &= ~sckBit;	// CHPA = 1/0
		
		// read the MISO pin and set the inData bit(x) accordingly if "read at Trailing edge" (CPOL = 1)
		// again, (x ^ bitOrder) inverts the bit selection if MSBFIRST is specified
		if (transferMode != SSPI_WRITE_ONLY && (dataMode & 0x01) && (*misoReg & misoBit)) {
			delayMicroseconds(SSPI_SETTLE_TIME);
			inData |= (1 << (x ^ bitOrder));
		}
		SREG = oldSREG;
	}
	return inData;
}

bool SSPIClass::makeRegMask(uint8_t thisPin, uint8_t **thisReg, uint8_t *thisBit, uint8_t inputOutput) {
  if(thisPin > (NUM_DIGITAL_PINS - 1) || inputOutput > MODEREG) return true;
	*thisBit = digitalPinToBitMask(thisPin);
	if(inputOutput == OUTPUT) *thisReg = portOutputRegister(digitalPinToPort(thisPin));
	else if(inputOutput == INPUT) *thisReg = portInputRegister(digitalPinToPort(thisPin));
	else if(inputOutput == MODEREG) *thisReg = portModeRegister(digitalPinToPort(thisPin)); 
	return false;
}
