// Advanced example: receive pictures from the included Python sender.
#include <ESTN_R2R.h>

ESTN_R2R xy;

// Connect the frame receiver to our drawing object and serial port.
ESTN_R2RSerial display(xy, Serial);

void setup()
{
  xy.begin();

  // The Python sender must use this same baud rate.
  Serial.begin(115200);
}

void loop()
{
  // Receive picture data and draw the next small part of the picture.
  // Call this frequently: do not add delay() here.
  display.update();
}
