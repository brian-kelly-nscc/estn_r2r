# Validation — 2026-09-23

All six examples compiled successfully with Arduino AVR Boards 1.8.8 for `arduino:avr:uno`.

| Example | Flash bytes | Static RAM bytes |
| --- | ---: | ---: |
| FirstShape | 1004 | 12 |
| ShapesAndDigits | 2336 | 42 |
| SerialCommands | 3308 | 227 |
| Objects | 1322 | 287 |
| BinarySerial | 2624 | 754 |
| CustomDrawing | 1036 | 12 |

Uno limits: 32,256 bytes of sketch flash and 2,048 bytes of RAM. Static RAM figures exclude the runtime stack. Keep frame/receiver instances global and avoid allocating many frames.

Host C++ tests passed with AddressSanitizer and UndefinedBehaviorSanitizer. Tests cover preserved port bits, coordinate saturation, line endpoints and adjacent samples, invalid digits, dwell limits, object limits, total capacity, valid FF payloads, incomplete-frame retention, malformed counts, idle-gap recovery, largest accepted packets, overflow rejection, empty frames, and disconnected-object traversal. Host tests use simulated Arduino registers and a serial stream; they do not prove physical timing or voltage behavior.

No board was uploaded and no oscilloscope measurements were taken. Before class, run the six examples on the actual two-ladder circuit, check axis orientation and all 16 output codes, and tune scope intensity / point dwell for readable pictures. Confirm repeated Python frames at the intended game rate and observe any serial loss or flicker.

The source and regression tests are delivered alongside this library. From the enclosing workspace:

```sh
clang++ -std=c++11 -Wall -Wextra -Werror -fsanitize=address,undefined \
  -D__AVR_ATmega328P__ -Itests/support -IESTN_R2R/src \
  tests/test_r2r.cpp ESTN_R2R/src/*.cpp -o /tmp/test-estn-r2r
/tmp/test-estn-r2r
arduino-cli compile --fqbn arduino:avr:uno --library "$PWD/ESTN_R2R" \
  ESTN_R2R/examples/05_BinarySerial
```

All six examples were recompiled successfully after the beginner-readability revision. Example 02 now uses sequential timed redraw loops instead of a switch expression; other example revisions expand formatting and explain each step. The library implementation is unchanged.
