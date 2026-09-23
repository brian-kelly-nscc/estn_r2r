#include "ESTN_R2R.h"
#include <util/atomic.h>

uint8_t ESTN_R2R::clamp(int v) { return v < 0 ? 0 : (v > 15 ? 15 : v); }
uint8_t ESTN_R2R::packPoint(int x, int y) { return (clamp(x) << 4) | clamp(y); }
void ESTN_R2R::begin() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    PORTB &= 0xF0; PORTC &= 0xF0;
    DDRB |= 0x0F; DDRC |= 0x0F;
  }
}
void ESTN_R2R::setXY(int x, int y) {
  uint8_t a = clamp(x), b = clamp(y);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    PORTB = (PORTB & 0xF0) | a;
    PORTC = (PORTC & 0xF0) | b;
  }
}
void ESTN_R2R::setPointDelay(unsigned int us) { dwell = us < 1 ? 1 : (us > 2000 ? 2000 : us); }
void ESTN_R2R::drawPoint(int x, int y) { setXY(x,y); delayMicroseconds(dwell); }
void ESTN_R2R::park() { setXY(0,0); }

void ESTN_R2R::drawLine(int x0, int y0, int x1, int y1)
{
  x0 = clamp(x0); y0 = clamp(y0);
  x1 = clamp(x1); y1 = clamp(y1);
  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);

  int steps = max(dx, dy);

  if (steps == 0)
  {
    setXY(x0, y0);
    delayMicroseconds(dwell);
    return;
  }

  for (int i = 0; i <= steps; i++)
  {
    int x = x0 + ((x1 - x0) * i) / steps;
    int y = y0 + ((y1 - y0) * i) / steps;

    setXY(x, y);
    delayMicroseconds(dwell);
  }
}


// ------------------------------------------------------------
// Seven-segment digit definitions
// ------------------------------------------------------------

const byte SEG_A = 0b0000001;
const byte SEG_B = 0b0000010;
const byte SEG_C = 0b0000100;
const byte SEG_D = 0b0001000;
const byte SEG_E = 0b0010000;
const byte SEG_F = 0b0100000;
const byte SEG_G = 0b1000000;

const byte digits[10] = {
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,          // 0
  SEG_B | SEG_C,                                          // 1
  SEG_A | SEG_B | SEG_G | SEG_E | SEG_D,                  // 2
  SEG_A | SEG_B | SEG_G | SEG_C | SEG_D,                  // 3
  SEG_F | SEG_G | SEG_B | SEG_C,                          // 4
  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,                  // 5
  SEG_A | SEG_F | SEG_G | SEG_E | SEG_C | SEG_D,          // 6
  SEG_A | SEG_B | SEG_C,                                  // 7
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,  // 8
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G           // 9
};


void ESTN_R2R::drawDigit(byte number)
{
  if (number > 9) return;
  byte pattern = digits[number];

  if (pattern & SEG_A) drawLine(4, 14, 12, 14);
  if (pattern & SEG_B) drawLine(12, 14, 12, 8);
  if (pattern & SEG_C) drawLine(12, 8, 12, 2);
  if (pattern & SEG_D) drawLine(12, 2, 4, 2);
  if (pattern & SEG_E) drawLine(4, 2, 4, 8);
  if (pattern & SEG_F) drawLine(4, 8, 4, 14);
  if (pattern & SEG_G) drawLine(4, 8, 12, 8);
}


// ------------------------------------------------------------
// Box
// ------------------------------------------------------------

void ESTN_R2R::drawBox()
{
  drawLine(2, 2, 13, 2);
  drawLine(13, 2, 13, 13);
  drawLine(13, 13, 2, 13);
  drawLine(2, 13, 2, 2);
}


// ------------------------------------------------------------
// Triangle
// ------------------------------------------------------------

void ESTN_R2R::drawTriangle()
{
  drawLine(2, 2, 13, 2);
  drawLine(13, 2, 8, 14);
  drawLine(8, 14, 2, 2);
}


// ------------------------------------------------------------
// Diamond
// ------------------------------------------------------------

void ESTN_R2R::drawDiamond()
{
  drawLine(8, 14, 12, 8);
  drawLine(12, 8, 8, 1);
  drawLine(8, 1, 4, 8);
  drawLine(4, 8, 8, 14);
}


// ------------------------------------------------------------
// Circle approximation
// ------------------------------------------------------------

void ESTN_R2R::drawCircle()
{
  const byte circle[][2] = {
    {8,15},
    {11,14},
    {13,12},
    {15,8},
    {14,5},
    {12,2},
    {8,1},
    {5,2},
    {2,5},
    {1,8},
    {2,11},
    {5,14},
    {8,15}
  };

  const int count = sizeof(circle) / sizeof(circle[0]);

  for (int i = 0; i < count - 1; i++)
  {
    drawLine(
      circle[i][0],
      circle[i][1],
      circle[i + 1][0],
      circle[i + 1][1]
    );
  }
}


// ------------------------------------------------------------
// Five-point star
// ------------------------------------------------------------

void ESTN_R2R::drawStar()
{
  drawLine(8, 15, 10, 10);
  drawLine(10, 10, 15, 10);
  drawLine(15, 10, 11, 7);
  drawLine(11, 7, 13, 1);
  drawLine(13, 1, 8, 5);
  drawLine(8, 5, 3, 1);
  drawLine(3, 1, 5, 7);
  drawLine(5, 7, 1, 10);
  drawLine(1, 10, 6, 10);
  drawLine(6, 10, 8, 15);
}


// ------------------------------------------------------------
// GO
// ------------------------------------------------------------

void ESTN_R2R::drawG()
{
  drawLine(7, 14, 3, 14);
  drawLine(3, 14, 2, 12);
  drawLine(2, 12, 2, 4);
  drawLine(2, 4, 3, 2);
  drawLine(3, 2, 7, 2);

  drawLine(7, 2, 7, 7);
  drawLine(7, 7, 5, 7);
}


void ESTN_R2R::drawO()
{
  drawLine(9, 14, 13, 14);
  drawLine(13, 14, 14, 12);
  drawLine(14, 12, 14, 4);
  drawLine(14, 4, 13, 2);
  drawLine(13, 2, 9, 2);
  drawLine(9, 2, 8, 4);
  drawLine(8, 4, 8, 12);
  drawLine(8, 12, 9, 14);
}


void ESTN_R2R::drawGO()
{
  drawG();
  drawO();
}



bool ESTN_R2RFrame::addObject(const uint8_t *points, uint8_t count) {
  if (!points || !count || count > MAX_OBJECT_POINTS ||
      objects >= MAX_OBJECTS || used + count > MAX_POINTS) return false;
  counts[objects++] = count;
  for (uint8_t i=0; i<count; ++i) data[used++] = points[i];
  return true;
}
void ESTN_R2R::drawObject(const uint8_t *points, uint8_t count) {
  if (!points || !count) return;
  if (count == 1) { drawPoint(points[0] >> 4, points[0] & 15); return; }
  for (uint8_t i=1; i<count; ++i)
    drawLine(points[i-1] >> 4, points[i-1] & 15, points[i] >> 4, points[i] & 15);
}
void ESTN_R2R::drawFrame(const ESTN_R2RFrame &f) {
  if (!f.objects) { park(); return; }
  uint16_t offset = 0;
  for (uint8_t i=0; i<f.objects; ++i) {
    drawObject(f.data + offset, f.counts[i]); offset += f.counts[i];
  }
}
void ESTN_R2R::printHelp(Print &p) {
  p.println(F("0-9: digit | B: box | T: triangle | D: diamond"));
  p.println(F("C: circle | S: star | G: GO | X: park | H: help"));
}
void ESTN_R2R::updateSerial(Stream &p) {
  // Bound work even when a sender keeps streaming.
  for (uint8_t n=0; n<32 && p.available(); ++n) {
    char c=p.read();
    if (c>='a' && c<='z') c-= 'a'-'A';
    if ((c>='0' && c<='9') || c=='B' || c=='T' || c=='D' ||
        c=='C' || c=='S' || c=='G' || c=='X') mode=c;
    else if (c=='H') printHelp(p);
  }
  drawCurrentMode();
}
void ESTN_R2R::drawCurrentMode() {
  if (mode>='0' && mode<='9') { drawDigit(mode-'0'); return; }
  switch(mode) {
    case 'B': drawBox(); break;
    case 'T': drawTriangle(); break;
    case 'D': drawDiamond(); break;
    case 'C': drawCircle(); break;
    case 'S': drawStar(); break;
    case 'G': drawGO(); break;
    default: park(); break;
  }
}
