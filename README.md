# arduino_SoftwareSPI
An alternative to SPI using Serial Bitbang

SoftwareSPI was primarily intended to work with shift registers and similar devices but it ended up working so like SPI that I decided to make its control, and use, SPI compatible (almost).
		
SoftwareSPI was originally written for the MAX7219 display device which uses a 16bit shift register	but it also worked well with the SSD1306 SPI display device so I thought I'd add the ability to	also read Serial data in. SPI should always be the first choice but if, like me, you want to be able to use alternatives to the fixed SPI pins, SoftwareSPI does the job.

SPI transfers are about 5 - 6 times faster than the equivalent SBI transfer but, the advantage of SBI is that you can use just about any digital pins in whatever order you wish.

SoftwareSPI uses the same pin names as SPI e.g. SCK, MISO and MOSI so, for clarity:

   SCK = CLOCK OUT  
   MISO = DATA IN  
   MOSI = DATA OUT

  SSPI Modes:

SSPI_MODE0 = SCK L-H-L, Clock on Leading Edge L > H  
SSPI_MODE1 = SCK L-H-L, Clock on Trailing Edge H > L  
SSPI_MODE2 = SCK H-L-H, Clock on Leading Edge H > L  
SSPI_MODE3 = SCK H-L-H, Clock on Trailing Edge L > H

  SSPI Bit Order:

  LSBFIRST  bits 76543210 ->  
  MSBFIRST  bits <- 76543210  
  
  Methods:
	// sets up the desired pins for INPUT/OUTPUT, SBI_MODE0 and MSBFIRST	
	begin(<SCK pin>, <MISO pin>, <MOSI pin>);
	
	// to send data only:
	begin(<SCK pin>, NOT_A_PIN, <MOSI pin>);	// use SCK and MOSI only
	
	// to read data only:
	begin(<SCK pin>, <MISO pin>, NOT_A_PIN);	// use SCK and MISO only
	
	// changes the default SBI_MODE0 and MSBFIRST	
	beginTransaction(<data mode>, <data order>);	
	
	example: beginTransaction(SBI_MODE1, LSBFIRST);
	
	// replaces begin() and beginTransaction() above	
	beginTransaction(<SCK pin>, <MISO pin>, <MOSI pin>, <data mode>, <data order>);
	
	// resets the SCK and MOSI pins to INPUTs	
	endTransaction(void);	
	
	// send and/or receive an 8 bit unsigned Byte out on MOSI. in on MISO	
	transfer(<data>);	
	
	// reads the MISO data whilst sending on MOSI	
	uint8_t inData = transfer(<data>);
	
	// creates a PIN/PORT/MODE register pointer and BIT mask for direct PORT manipulation	
	makeRegMask(<Arduino pin>, &<register>, &<bit>, <INPUT/OUTPUT/MODEREG>);
	

(C) Copyright 2018 Rev Phil Morris
