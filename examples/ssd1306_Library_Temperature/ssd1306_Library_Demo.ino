#include "Streaming.h"
#include <ssd1306SevenSegment.h>

#define DO  6 // Arduino pin D6
#define DI  5 // Arduino pin D5
#define RST 4 // Arduino pin D4
#define DC  3 // Arduino pin D3
#define CS  2 // Arduino pin D2

void setup()
{
  Serial.begin(57600);

  // using Bit-bang Serial instead of SPI
  // begin(clkPin, dinPin, rstPin, dcPin, csPin)
  ssd.begin(DO, DI, RST, DC, CS);

  // for SPI change "#define USE_BIT_BANG true" to "#define USE_BIT_BANG false"
  // at the start of "ssd1306SevenSegment.h"
  // clkPin to SCK, dinPin to MOSI
  // begin(rstPin, dcPin, csPin)
  //ssd.begin(9,8,10);
}

void loop()
{
  static int8_t t = -10;
  int8_t tmp;
  if(t < 0) {
    ssd.writeRaw(SSD1306_LARGE,0,0b00000001);  // digit 0 '-'
    tmp = -t;
  } else {
    ssd.writeRaw(SSD1306_LARGE,0,0); // digit 0 BLANK
    tmp = t;
  }

  ssd.writeDigit(SSD1306_LARGE,1,tmp/10,false); // digit 1 tens
  ssd.writeDigit(SSD1306_LARGE,2,tmp%10,false); // digit 2 ones
  ssd.writeRaw(SSD1306_LARGE,3,0b01100011);     // digit 3 DEGREE symbol
  delay(500);
  t++;
  if(t > 10) t = -10;
}
