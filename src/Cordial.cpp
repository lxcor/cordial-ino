#include "Cordial.h"

CordialTime cordialFromCrons(uint32_t crons) {
  crons %= CORDIAL_CRONS_IN_DAY;
  CordialTime t;
  t.hour = (uint8_t)(crons / CORDIAL_CRONS_IN_HOUR);
  crons %= CORDIAL_CRONS_IN_HOUR;
  t.minute = (uint8_t)(crons / CORDIAL_CRONS_IN_MINUTE);
  crons %= CORDIAL_CRONS_IN_MINUTE;
  t.second = (uint8_t)(crons / CORDIAL_CRONS_IN_SECOND);
  t.cron = (uint8_t)(crons % CORDIAL_CRONS_IN_SECOND);
  return t;
}

CordialTime cordialFromSecondsSinceNoon(uint32_t seconds) {
  return cordialFromCrons((uint32_t)(((uint64_t)(seconds % GREGORIAN_SECONDS_IN_DAY) * CORDIAL_CRONS_IN_DAY) / GREGORIAN_SECONDS_IN_DAY));
}

static uint32_t secondsSinceNoon(uint8_t hour, uint8_t minute, uint8_t second) {
  uint32_t secondOfDay = (uint32_t)hour * 3600UL + (uint32_t)minute * 60UL + second;
  return (secondOfDay + GREGORIAN_SECONDS_IN_DAY / 2) % GREGORIAN_SECONDS_IN_DAY;
}

CordialTime cordialFromGregorian(uint8_t hour, uint8_t minute, uint8_t second) {
  return cordialFromSecondsSinceNoon(secondsSinceNoon(hour, minute, second));
}

uint32_t cordialToGregorianSeconds(const CordialTime &t) {
  uint32_t crons = (uint32_t)t.hour * CORDIAL_CRONS_IN_HOUR + (uint32_t)t.minute * CORDIAL_CRONS_IN_MINUTE +
                   (uint32_t)t.second * CORDIAL_CRONS_IN_SECOND + t.cron;
  uint64_t numerator = (uint64_t)crons * GREGORIAN_SECONDS_IN_DAY;
  return (uint32_t)((numerator + CORDIAL_CRONS_IN_DAY - 1) / CORDIAL_CRONS_IN_DAY);   // ceiling
}

char *cordialFormat(const CordialTime &t, char *buf) {
  buf[0] = '0' + t.hour / 10;
  buf[1] = '0' + t.hour % 10;
  buf[2] = ':';
  buf[3] = '0' + t.minute / 10;
  buf[4] = '0' + t.minute % 10;
  buf[5] = ':';
  buf[6] = '0' + t.second / 10;
  buf[7] = '0' + t.second % 10;
  buf[8] = '.';
  buf[9] = '0' + t.cron;
  buf[10] = 0;
  return buf;
}

CordialClock::CordialClock(uint32_t frequencyHz) : _frequency(frequencyHz), _crons(0), _remainder(0) {}

void CordialClock::setCrons(uint32_t crons) {
  _crons = crons % CORDIAL_CRONS_IN_DAY;
  _remainder = 0;
}

void CordialClock::setGregorian(uint8_t hour, uint8_t minute, uint8_t second) {
  setCrons((uint32_t)(((uint64_t)secondsSinceNoon(hour, minute, second) * CORDIAL_CRONS_IN_DAY) / GREGORIAN_SECONDS_IN_DAY));
}

void CordialClock::addCycles(uint32_t cycles) {
  // crons = cycles * 492,128 / (frequency * 86,400); keep the remainder so nothing is lost
  const uint64_t perCron = (uint64_t)_frequency * GREGORIAN_SECONDS_IN_DAY;
  _remainder += (uint64_t)cycles * CORDIAL_CRONS_IN_DAY;
  const uint64_t whole = _remainder / perCron;
  _remainder -= whole * perCron;
  _crons = (uint32_t)((_crons + whole) % CORDIAL_CRONS_IN_DAY);
}
