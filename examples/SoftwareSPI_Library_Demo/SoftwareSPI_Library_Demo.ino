/* SoftwareSPI Interface demonstration

    A simple comparison between the standard SPI interface and this SSPI interface.

    SPI transfers are about 5 - 6 times faster than the equivalent SSPI transfer but, the
    advantage of SSPI is that you can use just about any digital pins in whatever order
    you wish. SSPI is primarily intended to work with shift registers and similar devices
    but it ended up working so like SPI that I decided to make its control, and use, SPI
    compatible (almost).

    MOSI is looped to MISO with a simple jumper wire for this demonstration and timings
    include the SS or Chip Select switching times. SSPI uses direct port access whereas
    SPI uses the more common wiring function, digitalWrite().

    SSPI also uses the same pin names as SPI e.g. SCK, MISO and MOSI so, for clarity:

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

    (C) Copyright 2018 Rev Phil Morris
*/
#define SSPI_MODE true  // true = SSPI, false = SPI

#include "Streaming.h"  // https://github.com/geneReeves/ArduinoStreaming

#if SSPI_MODE
#include <SoftwareSPI.h>
// this SSPI demo uses the UNO SPI pins for convenience
#define SCK_PIN 13
#define MISO_PIN 12
#define MOSI_PIN 11
#define CS_PIN 10
#define SELECT *csReg &= ~csBit;
#define DESELECT *csReg |= csBit;
#else
#include <SPI.h>
#define CS_PIN 10
#define SELECT digitalWrite(CS_PIN,LOW);
#define DESELECT digitalWrite(CS_PIN,HIGH);
#endif

uint8_t csBit, *csReg;  // Chip Select bit and register

void setup()
{
  Serial.begin(57600);

#if SSPI_MODE
  Serial << "SoftwareSPI Interface selected!" << endl;
  uint8_t success = SSPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);  // default SSPI_MODE0, MSBFIRST
  // or uint8_t success = SSPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SSPI_MODE0, MSBFIRST);
  if (success) {
    Serial << "SSPI error: " << success << " - ";
    switch (success) {
      case 1:
        Serial << "Invalid SCK pin!" << endl;
        break;
      case 2:
        Serial << "Invalid MISO pin!" << endl;
        break;
      case 3:
        Serial << "Invalid MOSI pin!" << endl;
        break;
      case 4:
        Serial << "MISO and MOSI pin not specified!" << endl;
    };
    while (true);
  }
  SSPI.makeRegMask(CS_PIN, &csReg, &csBit, OUTPUT);
#else
  Serial << "Serial Peripheral Interface selected!" << endl;
  SPI.begin();
  SPI.beginTransaction(SPISettings(F_CPU, SPI_MODE0, MSBFIRST));
#endif
  pinMode(CS_PIN, OUTPUT);
}

uint32_t startMicros, endMicros;

void loop()
{
  // Execution time SPI ~14 uS, SSPI ~70 uS
  static uint8_t counter = 0;
  startMicros = micros();
  SELECT
#if SSPI_MODE
  uint8_t result = SSPI.transfer(counter);
#else
  uint8_t result = SPI.transfer(counter);
#endif
  DESELECT
  endMicros = micros();
  Serial << "Sent: " << counter << "\tReceived: " << result << "\tin " << (endMicros - startMicros) << " uS" << endl;
  counter++;
  delay(500);
}
