// Host test (no Arduino needed): g++ -I src tests/host_test.cpp src/Cordial.cpp -o /tmp/cordial_host_test
// "dump" prints every second of the day for comparison with the Python core; the other checks assert directly.
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "Cordial.h"

static int failures = 0;
#define CHECK(c) do { if (!(c)) { printf("FAIL line %d: %s\n", __LINE__, #c); failures++; } } while (0)

int main(int argc, char **argv) {
  char buf[11];
  if (argc > 1 && !strcmp(argv[1], "dump")) {
    for (uint32_t s = 0; s < 86400; s++) {            // seconds since noon
      uint32_t day = (s + 43200) % 86400;
      CordialTime t = cordialFromGregorian(day / 3600, (day / 60) % 60, day % 60);
      printf("%02u:%02u:%02u %s\n", (unsigned)(day / 3600), (unsigned)((day / 60) % 60), (unsigned)(day % 60), cordialFormat(t, buf));
    }
    return 0;
  }
  CHECK(!strcmp(cordialFormat(cordialFromGregorian(12, 0, 0), buf), "00:00:00.0"));          // noon starts the Cordial day
  CHECK(!strcmp(cordialFormat(cordialFromGregorian(11, 59, 59), buf), "25:51:51.1"));        // the last second before noon (492,122 crons, truncated)
  CHECK(!strcmp(cordialFormat(cordialFromCrons(492127), buf), "25:51:51.6"));
  CHECK(cordialFromCrons(492128).hour == 0);                                                // wraps

  for (uint32_t s = 0; s < 86400; s += 7) {                                                 // round trip never loses a second
    CHECK(cordialToGregorianSeconds(cordialFromSecondsSinceNoon(s)) <= s);
    CHECK(cordialToGregorianSeconds(cordialFromSecondsSinceNoon(s)) + 1 >= s || s < 1);
  }

  // A 32.768 kHz crystal counted in chunks of 256 cycles (Timer2 overflow): exactly one day of cycles is exactly 492,128 crons.
  CordialClock watch(32768);
  for (uint32_t i = 0; i < 32768UL * 86400UL / 256UL; i++) watch.addCycles(256);
  CHECK(watch.crons() == 0);                                                                // wrapped after one whole day
  CordialClock hour(32768);
  for (uint32_t i = 0; i < 32768UL * 3600UL / 256UL; i++) hour.addCycles(256);
  CHECK(hour.crons() == 20505);                                                             // 3600 s * 492128 / 86400, truncated
  CordialClock odd(12000000);                                                               // any frequency
  for (int i = 0; i < 1000; i++) odd.addCycles(12000000);                                   // 1000 s at 12 MHz
  CHECK(odd.crons() == 5695);                                                               // 1000 x 5.6959 crons, the remainder carried

  printf(failures ? "FAILED %d\n" : "all checks passed\n", failures);
  return failures ? 1 : 0;
}
