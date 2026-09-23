# ESTN_R2R

Draw on an oscilloscope in XY mode using an Arduino Uno R3 and two 4-bit R-2R resistor ladders. The drawing area is a **16 × 16 grid**, with X and Y coordinates from **0 to 15**.

This library follows the classroom style shown in ESTN_WaveShield: include one header, create an object, call `begin()`, and call a drawing function in `loop()`.

## Install

1. In Arduino IDE, choose **Sketch → Include Library → Add .ZIP Library…** and select `ESTN_R2R.zip`.
2. Select **Arduino Uno** and your board's port.
3. Open **File → Examples → ESTN_R2R → 01_FirstShape** and upload it.

For manual installation, copy the `ESTN_R2R` folder into your sketchbook's `libraries` folder (typically `Documents/Arduino/libraries/` on macOS), then restart the IDE. No additional Arduino libraries are needed. This package is not published in Library Manager.

## Wiring

Keep the wiring used in the original demonstration. Each row connects to the corresponding bit input of its R-2R ladder.

| Bit | Weight | X ladder | Y ladder |
| --- | --- | --- | --- |
| 0 | 1 (least significant) | D8 / PB0 | A0 / PC0 |
| 1 | 2 | D9 / PB1 | A1 / PC1 |
| 2 | 4 | D10 / PB2 | A2 / PC2 |
| 3 | 8 (most significant) | D11 / PB3 | A3 / PC3 |

Connect the X ladder output to the scope's X input and the Y ladder output to its Y input. Connect the circuit and scope grounds together. Select XY mode and DC coupling; adjust scale and position to fit the display. Use high-impedance scope inputs. Exact voltages depend on the supply, resistors, and loading; coordinates are DAC codes, not volts.

This version targets the **ATmega328P Uno R3**. It does not support Uno R4, Mega, ESP32, or arbitrary pin mapping. Fixed port wiring keeps the implementation fast and makes classroom wiring consistent. It updates X then Y in two consecutive port writes; the updates are not perfectly simultaneous.

Reserve D8–D11 and A0–A3 for these ladders. Do not use `analogWrite()`, SPI, or another library that drives these same pins while drawing. Other port bits are preserved. The library uses no timer interrupt of its own and does not change timer settings.

## First picture

```cpp
#include <ESTN_R2R.h>

ESTN_R2R xy;

void setup() {
  xy.begin();
}

void loop() {
  xy.drawBox();
}
```

The Uno redraws the picture continuously. A drawing function draws once and returns. If you draw only in `setup()`, the picture fades and the output holds its final point. Long `delay()` calls in `loop()` leave a stationary dot rather than a persistent picture. Example 02 uses a simple sequence of timed `while` loops: redraw one picture for 1.5 seconds, then move to the next. Change `pictureDuration` to adjust that time. This introductory example stays inside each loop until its time is up; for a program that also handles buttons or serial commands, use a continuously serviced loop such as example 03 or 05.

## Drawing tools

| Call | What it does |
| --- | --- |
| `xy.begin()` | Sets the eight DAC pins as outputs, initially at zero |
| `xy.setXY(x, y)` | Immediately sets the DAC codes, without waiting |
| `xy.drawPoint(x, y)` | Sets a point and waits for one point dwell |
| `xy.drawLine(x0, y0, x1, y1)` | Interpolates between endpoints, including both |
| `xy.drawBox()` | Draws the original demonstration box |
| `xy.drawTriangle()` | Draws the original triangle |
| `xy.drawDiamond()` | Draws the original diamond |
| `xy.drawCircle()` | Draws the original polygonal circle approximation |
| `xy.drawStar()` | Draws the original star |
| `xy.drawDigit(number)` | Draws a seven-segment digit, 0–9; other values draw nothing |
| `xy.drawGO()` | Draws the original GO lettering |
| `xy.park()` | Holds both DAC outputs at zero |
| `xy.setPointDelay(us)` | Sets dwell per sample, limited to 1–2000 microseconds |
| `xy.pointDelay()` | Returns the current dwell setting |
| `ESTN_R2R::packPoint(x, y)` | Packs a coordinate into one byte |

Coordinates supplied to drawing functions are limited to 0–15: negative values become 0 and values above 15 become 15. Line endpoints are limited individually; this is not geometric line clipping. The original sketch wrapped values with a bit mask instead. The default dwell remains **120 microseconds**.

Longer dwell can brighten individual points but slows refresh and can increase flicker. More lines and objects also slow refresh. A zero-length line draws one point. Shape sizes are deliberately fixed to match the tested demonstration; use lines or objects to create custom shapes.

**There is no beam blanking connection.** `park()` does not turn the beam off; it leaves a dot at `(0,0)`. Moving between disconnected segments is a quick voltage jump, which can still produce visible retrace. Lower the scope intensity if a stationary dot is too bright. Separate objects avoid deliberately interpolating connecting lines, but cannot guarantee invisible travel.

## Serial Monitor: digits and shapes

Upload **03_SerialCommands**, open Serial Monitor at **115200 baud**, and send one command:

| Command | Picture |
| --- | --- |
| `0`–`9` | Digit |
| `B` | Box |
| `T` | Triangle |
| `D` | Diamond |
| `C` | Circle |
| `S` | Star |
| `G` | GO |
| `X` | Park at zero (the old “blank” command) |
| `H` | Print help |

Lowercase letters also work. Line endings and unknown characters are ignored. The initial picture is zero. `xy.updateSerial(Serial)` reads commands and redraws the selected picture once. This interface is intended for human-paced commands; it does not acknowledge each command. The sketch, not the library, starts Serial.

## Make your own objects

An object is a list of packed points joined in order. One point produces a dot. To close a shape, repeat the first point at the end. A frame is a collection of independent objects.

```cpp
#include <ESTN_R2R.h>
ESTN_R2R xy;
ESTN_R2RFrame frame;

void setup() {
  xy.begin();
  const uint8_t path[] = {
    xy.packPoint(2, 2), xy.packPoint(8, 14), xy.packPoint(13, 2)
  };
  if (!frame.addObject(path, sizeof(path))) {
    frame.clear();
  }
}

void loop() {
  xy.drawFrame(frame);
}
```

`frame.addObject(points, count)` copies a RAM array and returns `true` on success. A failed addition leaves the frame unchanged. Limits are **16 objects, 64 points per object, and 256 total points**. Empty objects and null pointers are rejected. `frame.clear()` removes all objects; `frame.objectCount()` reports how many are stored. An empty frame parks the outputs.

`xy.drawObject(points, count)` draws a RAM array directly without storing a frame. Zero points or a null pointer do nothing. `xy.drawFrame(frame)` draws each object once. Neither function closes paths automatically. Arrays passed to these functions must be in ordinary RAM, not `PROGMEM`.

## Binary serial display (Python / Snake)

Upload **05_BinarySerial**. It uses a separate receiver:

```cpp
ESTN_R2R xy;
ESTN_R2RSerial display(xy, Serial);
```

Call `xy.begin()` and `Serial.begin(115200)` in `setup()`, then call `display.update()` repeatedly in `loop()`. Keep the loop short and do not combine this receiver with `updateSerial()` or long drawing functions. Only one consumer should read the serial stream.

The wire format matches the multi-object version from the Snake conversation:

```text
0xFF  object_count
      point_count  packed_point ...
      point_count  packed_point ...
      ...
packed_point = (X << 4) | Y
```

For example, a line from `(2,2)` to `(13,13)` plus food at `(8,8)` is:

```text
FF 02 02 22 DD 01 88
```

Counts obey the same limits as local frames. `FF 00` clears the frame and parks the output. A point at `(15,15)` is `FF`; this is valid inside point data. The marker is recognized only between packets. This is **not** the older single-object packet format or the text `F x,y ...` format.

The receiver keeps showing the last complete frame while it receives the next one. It draws one interpolated sample per update, allowing serial input to be serviced between samples. Only complete valid packets replace the displayed frame. It uses two fixed buffers and no heap allocation.

For each accepted frame, the Uno sends one ASCII **`K`** byte. Send one packet, wait for `K`, then send the next. This acknowledgement is an addition to the old demo protocol. Older senders that do not read it should be updated for sustained use. There are no startup messages on the binary stream.

An incomplete packet expires after more than **100 ms** without a received byte. Invalid counts cause the receiver to discard input until the same idle gap. The sender should transmit each packet as one contiguous write. If an acknowledgement is missing, wait at least 150 ms with no transmission, discard stale incoming bytes, and retry a complete packet. This simple format has no checksum or sequence number: a corrupted coordinate may go undetected. It is suitable for the classroom USB serial demonstration, not a guaranteed-delivery protocol.

The earlier demo reserved 16 × 64 points. This library intentionally limits the **combined** frame to 256 points so it can keep both the old and incoming frame safely within Uno RAM. Existing Snake frames within these limits retain the same encoding. Split a snake into separate objects at wrap boundaries to avoid a long interpolated line across the screen.

### Try the included sender on macOS

Close Serial Monitor first so Python can open the port. From this library folder:

```sh
python3 -m venv .venv
source .venv/bin/activate
python -m pip install pyserial
python extras/send_frame.py --port /dev/cu.usbmodem2101
```

Use your board's current port if it differs. The sender waits for the Uno's reset, sends a box and a separate center point, and checks the acknowledgement. The board keeps redrawing the accepted frame after the sender exits.

## Examples

1. **01_FirstShape** — simplest possible box.
2. **02_ShapesAndDigits** — change pictures without stopping redraw.
3. **03_SerialCommands** — the original keyboard-controlled demo.
4. **04_Objects** — snake body and a separate food point.
5. **05_BinarySerial** — computer-controlled frames.
6. **06_CustomDrawing** — draw your own picture with lines and points.

## Troubleshooting

- **Only a dot:** keep drawing in `loop()`; check XY mode, both ladders, and scope scale.
- **Mirrored picture:** check scope inversion and ladder bit order.
- **Strange coordinate jumps:** confirm bit 0 versus bit 3 wiring and resistor connections.
- **Faint connecting lines:** expected without beam blanking; objects reduce dwell during travel but cannot remove it completely.
- **Flicker:** reduce picture complexity or lower point dwell; do not pause between redraws.
- **No serial response:** check the sketch (text versus binary), port, baud rate, and whether another program has the port open.
- **Unsupported board error:** select an ATmega328P Uno R3. Arbitrary pin mapping is not provided in this version.

