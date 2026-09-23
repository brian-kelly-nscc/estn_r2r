#include "ESTN_R2R.h"

ESTN_R2RSerial::ESTN_R2RSerial(ESTN_R2R &d, Stream &p)
  : xy(d), port(p), active(0), state(0), expected(0), remaining(0),
    lastByte(0), object(0), segment(0), step(0), offset(0) {}

void ESTN_R2RSerial::commit() {
  active ^= 1;
  object = segment = step = 0; offset = 0;
  state = 0;
  port.write('K'); // Acknowledge a complete, accepted frame.
}
void ESTN_R2RSerial::receive(uint8_t v) {
  ESTN_R2RFrame &f = frames[active ^ 1];
  switch (state) {
    case 0: // Marker is special ONLY between packets.
      if (v == 0xFF) { f.clear(); state = 1; }
      break;
    case 1:
      if (v > ESTN_R2RFrame::MAX_OBJECTS) { state = 4; break; }
      expected = v;
      if (!v) commit(); else state = 2;
      break;
    case 2:
      if (!v || v > ESTN_R2RFrame::MAX_OBJECT_POINTS ||
          f.used + v > ESTN_R2RFrame::MAX_POINTS) { state = 4; break; }
      f.counts[f.objects++] = v; remaining = v; state = 3;
      break;
    case 3:
      f.data[f.used++] = v;
      if (!--remaining) {
        if (f.objects == expected) commit(); else state = 2;
      }
      break;
    default: break; // Discard malformed packet until an idle gap.
  }
}
void ESTN_R2RSerial::update() {
  // Timeout uses unsigned subtraction, including across millis() rollover.
  if (state && (unsigned long)(millis() - lastByte) > 100UL) state = 0;
  for (uint8_t n=0; n<64 && port.available(); ++n) {
    int v=port.read();
    if (v < 0) break;
    lastByte=millis(); receive((uint8_t)v);
  }
  sample();
}
void ESTN_R2RSerial::sample() {
  const ESTN_R2RFrame &f=frames[active];
  if (!f.objects) { xy.park(); return; }
  uint8_t count=f.counts[object];
  uint8_t a=f.data[offset + segment];
  uint8_t b=count == 1 ? a : f.data[offset + segment + 1];
  int x=a >> 4, y=a & 15;
  int dx=(b >> 4)-x, dy=(b & 15)-y;
  int steps=max(abs(dx),abs(dy));
  xy.drawPoint(x + (steps ? dx*step/steps : 0), y + (steps ? dy*step/steps : 0));
  if (++step <= steps) return;
  step=0;
  if (count > 1 && ++segment < count-1) return;
  segment=0; offset += count;
  if (++object >= f.objects) { object=0; offset=0; }
}
