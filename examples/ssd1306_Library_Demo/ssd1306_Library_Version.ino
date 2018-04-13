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
  // write 16 small digits with decimal points
  for (uint8_t number = 0; number < 16; number++) {
    ssd.writeDigit(SSD1306_SMALL, number, number, true);
  }
  delay(2000);
  // write special characters
  ssd.clearDisplay();
  ssd.writeDigit(SSD1306_SMALL, 0, '=', false);
  ssd.writeRaw(SSD1306_SMALL, 1, 0b00000001); // HYPHEN
  ssd.writeDigit(SSD1306_SMALL, 2, ':', false);
  ssd.writeDigit(SSD1306_SMALL, 3, '+', false);
  delay(2000);

  // write 16 large digits. 4 at a time with decimal points
  uint8_t number = 0;
  for (uint8_t repeat = 0; repeat < 4; repeat++) {
    for (uint8_t digit = 0; digit < 4; digit++) {
      ssd.writeDigit(SSD1306_LARGE, digit, number++, true);
    }
    delay(2000);
  }
  // write special characters
  ssd.writeDigit(SSD1306_LARGE, 0, '=', false);
  ssd.writeRaw(SSD1306_SMALL, 1, 0b00000001); // HYPHEN
  ssd.writeDigit(SSD1306_LARGE, 2, ':', false);
  ssd.writeDigit(SSD1306_LARGE, 3, '+', false);
  delay(2000);
  ssd.clearDisplay();
  // write segments A,D and G
  ssd.writeRaw(SSD1306_LARGE, 0, 0b01001001);
  delay(1000);
  // write segments B and F
  ssd.writeRaw(SSD1306_LARGE, 1, 0b00100010);
  delay(1000);
  // write segments C and E
  ssd.writeRaw(SSD1306_LARGE, 2, 0b00010100);
  delay(1000);
  // write segments B,C,E and F
  ssd.writeRaw(SSD1306_LARGE, 3, 0b00110110);
  delay(2000);
}
