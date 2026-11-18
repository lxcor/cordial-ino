// Shows the Cordial time on the 16x2 LCD of the Arduino Starter Kit (the wiring of its project 11, "Crystal Ball").
// Starts from the compile time and counts with millis(); see SerialTime for the caveat.
#include <LiquidCrystal.h>
#include <Cordial.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
CordialClock clockCounter(1000);
unsigned long last;

static uint8_t compileDigits(int i) { return (__TIME__[i] - '0') * 10 + (__TIME__[i + 1] - '0'); }

void setup() {
  lcd.begin(16, 2);
  lcd.print("Cordial time");
  clockCounter.setGregorian(compileDigits(0), compileDigits(3), compileDigits(6));
  last = millis();
}

void loop() {
  unsigned long now = millis();
  clockCounter.addCycles(now - last);
  last = now;
  char text[11];
  lcd.setCursor(0, 1);
  lcd.print(cordialFormat(clockCounter.time(), text));
  delay(50);
}
