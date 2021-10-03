#include "SFM3300.h"
#include <Wire.h>

SFM3300::SFM3300(uint8_t i2c_addr)
{
  SFM3300_Address = i2c_addr;
}

void SFM3300::init()
{
  Wire.begin();

  // Soft reset Command 0x2000
  Wire.beginTransmission(SFM3300_Address);
  Wire.write(0x20);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(100);

  // Start Flow Measurement Command 0x1000
  Wire.beginTransmission(SFM3300_Address);
  Wire.write(0x10);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(100);
}

void SFM3300::reset() {
  asm volatile ("jmp 0");
}

typedef enum {
  CHECKSUM_ERROR = 0X04
} etError;

#define POLYNOMIAL 0x131   //P(x)=x^8+x^5+x^4+1 = 100110001
//=============================================================================
uint8_t SMF3300_CheckCrc (uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum)
//=============================================================================
// calculates checksum for n bytes of data
// and compares it with expected checksum
// Input:       data[]      checksum is built based on this data
//              nbrOfBytes  checksum is built for n bytes of data
//              checksum    expected checksum
// Return:      error:      CHECKSUM_ERROR = checksum does not match
//                          0 = checksum matches
//=============================================================================
{
  uint8_t crc = 0;
  uint8_t byteCtr;
  // Calculates 8-Bit checksum with given polynomial
  for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr) {
    crc ^= (data[byteCtr]);
    for (uint8_t bit = 8; bit > 0; --bit) {
      if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
      else crc = (crc << 1);
    }
  }
  if (crc != checksum) return CHECKSUM_ERROR;
  else return 0;
}

int count = 0;

int SFM3300::getValue()
{
  if (3 == Wire.requestFrom(SFM3300_Address, 3)) {
    count = 0;
    uint8_t readByte[2];
    uint16_t a = readByte[0] = Wire.read();
    uint8_t  b = readByte[1] = Wire.read();
    uint8_t  crc = Wire.read();
    if (SMF3300_CheckCrc(readByte, 2, crc)) {
      //Serial.println("Error: wrong CRC");
    }
    a = (a << 8) | b;
    int Flow = a;
    return Flow;
  }
  else {
    // Failed sensor reading
    count ++;
    if (count >= 2) {
      // Software reset the microcontroller unit
      asm volatile ("jmp 0");
    }
  }
}

void SFM3300::hardReset(uint8_t sensorPowerPin)
{
  pinMode(sensorPowerPin, OUTPUT);
  for (;;) {
    // Switch the NPN BJT transistor to hard reset the sensor
    digitalWrite(sensorPowerPin, LOW);
    delay(500);
    digitalWrite(sensorPowerPin, HIGH);
    delay(100);
    break;
  }
}