#ifndef ESTN_R2R_H
#define ESTN_R2R_H
#include <Arduino.h>
#if !defined(__AVR_ATmega328P__)
#error "ESTN_R2R requires an ATmega328P (Arduino Uno R3). Uno R4 is not supported."
#endif

// A packed, RAM-resident frame. Separate objects are separate polylines.
class ESTN_R2RFrame {
public:
  enum { MAX_OBJECTS = 16, MAX_POINTS = 256, MAX_OBJECT_POINTS = 64 };
  ESTN_R2RFrame() : objects(0), used(0) {}
  void clear() { objects = 0; used = 0; }
  bool addObject(const uint8_t *points, uint8_t count);
  uint8_t objectCount() const { return objects; }
private:
  friend class ESTN_R2R;
  friend class ESTN_R2RSerial;
  uint8_t objects;
  uint16_t used;
  uint8_t counts[MAX_OBJECTS];
  uint8_t data[MAX_POINTS];
};

class ESTN_R2R {
public:
  ESTN_R2R() : dwell(120), mode('0') {}
  void begin();
  void setXY(int x, int y); // Saturates to 0..15; does not wait.
  void setPointDelay(unsigned int microseconds); // 1..2000 us
  unsigned int pointDelay() const { return dwell; }
  void drawPoint(int x, int y);
  void drawLine(int x0, int y0, int x1, int y1);
  void drawBox();
  void drawTriangle();
  void drawDiamond();
  void drawCircle();
  void drawStar();
  void drawDigit(uint8_t number);
  void drawGO();
  void park(); // Holds (0,0). No hardware blanking is available.
  void drawObject(const uint8_t *points, uint8_t count);
  void drawFrame(const ESTN_R2RFrame &frame);
  void updateSerial(Stream &port); // Text commands + one redraw.
  void printHelp(Print &port);
  static uint8_t packPoint(int x, int y);
private:
  static uint8_t clamp(int value);
  void drawG();
  void drawO();
  void drawCurrentMode();
  unsigned int dwell;
  char mode;
};

// Optional binary receiver: owns two small buffers. Declare globally.
class ESTN_R2RSerial {
public:
  ESTN_R2RSerial(ESTN_R2R &display, Stream &port);
  void update(); // Call repeatedly; receives bytes and draws one sample.
private:
  void receive(uint8_t value);
  void commit();
  void sample();
  ESTN_R2R &xy;
  Stream &port;
  ESTN_R2RFrame frames[2];
  uint8_t active, state, expected, remaining;
  unsigned long lastByte;
  uint8_t object, segment, step;
  uint16_t offset;
};
#endif
