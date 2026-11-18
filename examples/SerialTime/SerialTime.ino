// Prints the Cordial time to the serial monitor once a Cordial second.
// The Uno has no real-time clock, so the sketch starts from the time it was compiled and counts with millis():
// good for trying the library, it drifts like the Uno's ceramic resonator does (use a 32.768 kHz crystal for a real clock).
#include <Cordial.h>

CordialClock clockCounter(1000);            // millis() ticks at 1000 Hz
unsigned long last;

static uint8_t compileDigits(int i) { return (__TIME__[i] - '0') * 10 + (__TIME__[i + 1] - '0'); }

void setup() {
  Serial.begin(9600);
  clockCounter.setGregorian(compileDigits(0), compileDigits(3), compileDigits(6));   // __TIME__ is "HH:MM:SS"
  last = millis();
}

void loop() {
  unsigned long now = millis();
  clockCounter.addCycles(now - last);
  last = now;
  static uint32_t shown = 0xFFFFFFFF;
  uint32_t second = clockCounter.crons() / CORDIAL_CRONS_IN_SECOND;
  if (second != shown) {
    shown = second;
    char text[11];
    Serial.println(cordialFormat(clockCounter.time(), text));
  }
}
