#ifndef SFM3300_h
#define SFM3300_h

#include <Arduino.h>

class SFM3300 {
  public:
    SFM3300(uint8_t i2c_addr);
    void begin();
    int getValue();
    void hardReset(uint8_t sensorPowerPin);
    void reset();
  private:
    int SFM3300_Address;
};

#endif
