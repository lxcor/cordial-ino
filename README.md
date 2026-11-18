# Cordial for Arduino

The Cordial clock as an Arduino library: 26 hours of 52 minutes of 52 seconds, seven crons to a Cordial second, the day starting at noon.
Integer arithmetic only (no floats, no heap).

```cpp
#include <Cordial.h>

CordialTime t = cordialFromGregorian(14, 30, 0);   // 02:30 pm  ->  Cordial time
char text[11];
Serial.println(cordialFormat(t, text));            // "HH:MM:SS.c"
```

`CordialClock` counts the cycles of any crystal into crons and carries the remainder, so the count is exact on average
(a 32.768 kHz watch crystal included). Examples: `SerialTime`, and `LcdClock` for the 16x2 LCD of the Arduino Starter Kit.

## Tests
`g++ -I src tests/host_test.cpp src/Cordial.cpp -o /tmp/t && /tmp/t` runs on any computer. All 86,400 seconds of a day were compared
with the Python reference (`lxcor-cordial`), with no difference.

Licence: Apache 2.0 (see LICENSE and NOTICE).
