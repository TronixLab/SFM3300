#include <SFM3300.h>

#define resetDPin            2
#define SFM3300_addr         0x40
#define SFM3300_offSet       32768.00D
#define SFM3300_scaleFactor  120.00D
#define delayPeriod          50

SFM3300 sensirionFlow(SFM3300_addr);

double airFlow = 0;
double tidalVolume = 0;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  sensirionFlow.hardReset(resetDPin);
  sensirionFlow.init();
}

void loop() {
  unsigned int rawData = sensirionFlow.getValue();
  // Calculate air flow rate in Standard Litter per minute (SLM)
  airFlow = ((double(rawData) - SFM3300_offSet) / SFM3300_scaleFactor);

  if (floor(airFlow) == -273 || airFlow == 57.65D) {
    // Software reset the microcontroller unit
    sensirionFlow.reset();
  }

  // Check the flow triggers (inspiration and expiration):
  if (abs(airFlow) > 0.50D) {
    // Calculate the tidal volume in mL @ every 50ms:
    tidalVolume = tidalVolume + ((airFlow / 60) * (millis() - previousMillis));
    previousMillis = millis();
    if (tidalVolume < 0.00) {
      tidalVolume = 0.00;
    }
  }
  else {
    tidalVolume = 0.00;
  }

  // Normalized the waveform for plotting
  double normFlow = airFlow / 30;
  double normVol = tidalVolume / 200;

  // Plot the normilized waveform; use serial monitor
  Serial.print(normFlow);
  Serial.print(",");
  Serial.print(normVol);
  Serial.println();
  delay(delayPeriod);
}
