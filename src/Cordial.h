// Cordial: the Cordial clock for Arduino.
// A Cordial day is 26 hours of 52 minutes of 52 seconds (70,304 Cordial seconds), seven crons to a Cordial second
// (492,128 crons), and it starts at noon. Everything here is integer arithmetic: no floats, no heap.
#ifndef CORDIAL_H
#define CORDIAL_H

#include <stdint.h>

const uint32_t CORDIAL_SECONDS_IN_DAY = 70304UL;
const uint32_t CORDIAL_CRONS_IN_SECOND = 7UL;
const uint32_t CORDIAL_CRONS_IN_MINUTE = 364UL;      // 52 seconds
const uint32_t CORDIAL_CRONS_IN_HOUR = 18928UL;      // 52 minutes
const uint32_t CORDIAL_CRONS_IN_DAY = 492128UL;      // 26 hours
const uint32_t GREGORIAN_SECONDS_IN_DAY = 86400UL;

struct CordialTime {
  uint8_t hour;     // 0 to 25
  uint8_t minute;   // 0 to 51
  uint8_t second;   // 0 to 51
  uint8_t cron;     // 0 to 6
};

// Cordial time from the crons counted since the Cordial day began (noon), 0 to 492,127.
CordialTime cordialFromCrons(uint32_t crons);

// The same from Gregorian seconds since noon (0 to 86,399), truncated to the cron.
CordialTime cordialFromSecondsSinceNoon(uint32_t seconds);

// The same from a Gregorian time of day, 00:00:00 to 23:59:59 (the Cordial day began at the preceding noon).
CordialTime cordialFromGregorian(uint8_t hour, uint8_t minute, uint8_t second);

// Gregorian seconds since noon at which a Cordial time begins (the first whole second in or after it).
uint32_t cordialToGregorianSeconds(const CordialTime &t);

// Writes "HH:MM:SS.c" into buf (at least 11 bytes, the last one the terminator). Returns buf.
char *cordialFormat(const CordialTime &t, char *buf);

// A clock that counts the cycles of a crystal into crons. The remainder of every division is carried, so the count is
// exact on average for any crystal frequency, a 32.768 kHz watch crystal included.
class CordialClock {
 public:
  explicit CordialClock(uint32_t frequencyHz);

  void setCrons(uint32_t crons);          // set the time (crons since noon); also clears the carried remainder
  void setGregorian(uint8_t hour, uint8_t minute, uint8_t second);
  void addCycles(uint32_t cycles);        // call with the cycles counted since the last call
  uint32_t crons() const { return _crons; }
  CordialTime time() const { return cordialFromCrons(_crons); }

 private:
  uint32_t _frequency;
  uint32_t _crons;
  uint64_t _remainder;                    // in units of 1 / (frequency * 86,400) crons
};

#endif
