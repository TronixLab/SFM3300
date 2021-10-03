#include <SFM3300.h>

#define resetDPin            2
#define SFM3300_addr         0x40
#define SFM3300_offSet       32768.00D
#define SFM3300_scaleFactor  120.00D
#define delayPeriod          50

SFM3300 sensirionFlow(SFM3300_addr);

void setup() {
  Serial.begin(115200);
  sensirionFlow.hardReset(resetDPin);
  sensirionFlow.begin();
}

void loop() {
  unsigned int rawData = sensirionFlow.getValue();
  // Calculate air flow rate in Standard Litter per minute (SLM)
  double flow = ((double(rawData) - SFM3300_offSet) / SFM3300_scaleFactor);
  Serial.println(flow);

  if (floor(flow) == -273 || flow == 57.65D) {
    // Software reset the microcontroller unit
    sensirionFlow.reset();
  }
  delay(delayPeriod);
}
