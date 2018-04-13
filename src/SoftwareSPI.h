#ifndef SOFTWARESPI_H
#define SOFTWARESPI_H

/* Serial Bit-bang Interface with SPI demonstration

    SBI is primarily intended to work with shift registers and similar devices
    but it ended up working so like SPI that I decided to make its control, and use, SPI
    compatible (almost).
		
		SBI was originally written for the MAX7219 display device which uses a 16bit shift register
		but it also worked well with the SSD1306 SPI display device so I thought I'd add the ability to
		also read Serial data in. SPI should always be the first choice but if, like me, you want to
		be able to use alternatives to the fixed SPI pins, SBI does the job.

    SPI transfers are about 5 - 6 times faster than the equivalent SBI transfer but, the
    advantage of SBI is that you can use just about any digital pins in whatever order
    you wish.

    SBI uses the same pin names as SPI e.g. SCK, MISO and MOSI so, for clarity:

     SCK = CLOCK OUT
    MISO = DATA IN
    MOSI = DATA OUT

    SBI Modes:

    SSPI_MODE0 = SCK L-H-L, Clock on Leading Edge L > H
    SSPI_MODE1 = SCK L-H-L, Clock on Trailing Edge H > L
    SSPI_MODE2 = SCK H-L-H, Clock on Leading Edge H > L
    SSPI_MODE3 = SCK H-L-H, Clock on Trailing Edge L > H

    SSPI Bit Order:

    LSBFIRST  bits 76543210 ->
    MSBFIRST  bits <- 76543210
		
		Methods:
		
		begin(<SCK pin>, <MISO pin>, <MOSI pin>);	// sets up the desired pins for INPUT/OUTPUT, SSPI_MODE0 and MSBFIRST
		// to send data only:
		begin(<SCK pin>, NOT_A_PIN, <MOSI pin>);	// use SCK and MOSI only
		// to read data only:
		begin(<SCK pin>, <MISO pin>, NOT_A_PIN);	// use SCK and MISO only
		
		beginTransaction(<data mode>, <data order>);	// changes the default SSPI_MODE0 and MSBFIRST
		// example
		beginTransaction(SSPI_MODE1, LSBFIRST);
		
		beginTransaction(<SCK pin>, <MISO pin>, <MOSI pin>, <data mode>, <data order>);
		// replaces begin() and beginTransaction() above
		
		endTransaction(void);	// restes the SCK and MOSI pins to INPUTs
		
		transfer(<data>);	// send an 8 bit unsigned Byte out on MOSI
		
		uint8_t inData = transfer(<data>);	// reads the MISO data whilst sending on MOSI
		
		makeRegMask(<Arduino pin>, <register>, <bit>, <INPUT/OUTPUT>);
		// creates a PORT and BIT mask for direct PORT manipulation

		//Blink example for UNO, NANO etc. with LED and Serial output:
		
		#include <SBI.h>
		
		volatile uint8_t *portReg, portBit, *pinReg, pinBit;
		uint32_t startMicros, endMicros;
		
		void setup() {
			Serial.begin(57600);
			SBI.makeRegMask(LED_BUILTIN, &portReg, &portBit, OUTPUT);	// *portReg = PORTB, portBit = PB5 bitmask
			SBI.makeRegMask(LED_BUILTIN, &pinReg, &pinBit, INPUT);		// *pinReg = PINB, pinBit = PB5 bitmask
			pinMode(LED_BUILTIN, OUTPUT);
		}
		
		void loop() {
			startMicros = micros();
			*portReg |= portBit;	// LED ON
			endMicros = micros();
			Serial.print(*pinReg & pinBit? "HIGH":"LOW");	// read the state of the LED pin and print it
			delay(1000);
			*portReg &= ~portBit;	// LED OFF
			Serial.println(*pinReg & pinBit? "HIGH":"LOW");	// read the state of the LED pin and print it
			delay(1000);
		}
		
		The advantage of using direct port manipulation is speed, not memory saving 

    (C) Copyright 2018 Rev Phil Morris
*/


#include <Arduino.h>

#define SSPI_READ_WRITE	0x00
#define SSPI_WRITE_ONLY	0x01
#define SSPI_READ_ONLY		0x02

#define SSPI_MODE0 0x00	//	   0	SCK L-H-L Clock on Leading Edge
#define SSPI_MODE1 0x04	//	 100	SCK L-H-L	Clock on Trailing Edge
#define SSPI_MODE2 0x08	//	1000	SCK	H-L-H	Clock on Leading Edge
#define SSPI_MODE3 0x0C	//	1100	SCK H-L-H	Clock on Trailing Edge

// Belt and Braces
#ifndef LSBFIRST
#define LSBFIRST 0x00
#endif
#ifndef MSBFIRST
#define MSBFIRST 0x01
#endif
#ifndef NOT_A_PIN
#define NOT_A_PIN -1
#endif
#ifndef OUTPUT
#define OUTPUT 0x00
#endif
#ifndef INPUT
#define INPUT 0x01
#endif

#define MODEREG	0x02

#define SSPI_SETTLE_TIME	0	// signal settle delay in uS to delay reading of incoming data bits

class SSPIClass {
public:

uint8_t begin(uint8_t _sck, int8_t _miso, int8_t _mosi);
void beginTransaction(uint8_t _dataMode, uint8_t _dataOrder);
uint8_t beginTransaction(uint8_t _sck, int8_t _miso, int8_t _mosi, uint8_t _dataMode, uint8_t _dataOrder);
void endTransaction(void);
uint8_t transfer(uint8_t outData);
bool makeRegMask(uint8_t thisPin, uint8_t **thisReg, uint8_t *thisBit, uint8_t inputOutput);

	volatile uint8_t sckBit, *sckReg, misoBit, *misoReg, mosiBit, *mosiReg, dataMode, bitOrder;
	int8_t transferMode, sckPin, mosiPin;
};

extern SSPIClass SSPI;

#endif