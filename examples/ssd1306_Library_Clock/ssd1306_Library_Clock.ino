#include "Streaming.h"
#include <TimeLib.h>
#include <ssd1306SevenSegment.h>

#define DO  6 // Arduino pin D6
#define DI  5 // Arduino pin D5
#define RST 4 // Arduino pin D4
#define DC  3 // Arduino pin D3
#define CS  2 // Arduino pin D2

time_t timeSeed = 1521784957UL; // UNIX timestamp

void setup()
{
  //Serial.begin(57600);

  // using Bit-bang Serial instead of SPI
  // begin(clkPin, dinPin, rstPin, dcPin, csPin)
  ssd.begin(DO, DI, RST, DC, CS);

  // for SPI change "#define USE_BIT_BANG true" to "#define USE_BIT_BANG false"
  // at the start of "ssd1306SevenSegmentSBI.h"
  // clkPin to SCK, dinPin to MOSI
  /*
    begin(rstPin, dcPin, csPin)
    ssd.begin(9,8,10);
  */

  setTime(timeSeed);
}

void loop()
{
  static uint8_t lastSecond = 0xFF; // invalid value to force change
  static uint8_t lastDay = 0xFF; // invalid value to force change
  uint8_t tBuff[8];
  if (second() != lastSecond) {
    lastSecond = second();
    sprintf(tBuff, "%02u:%02u:%02u", hour(), minute(), second());
    for (uint8_t t = 0; t < 8; t++) ssd.writeDigit(SSD1306_SMALL, t, tBuff[t], false);
  }
  // if the day changes, re-print it
  if (day() != lastDay) {
    lastDay = day();
    sprintf(tBuff, "%02u-%02u-%02u", day(), month(), year() - 2000);
    for (uint8_t t = 0; t < 8; t++) ssd.writeDigit(SSD1306_SMALL, t + 8, tBuff[t], false);
  }
}
