// Load the drawing library.
#include <ESTN_R2R.h>

// Create an object named xy to control our two R-2R circuits.
ESTN_R2R xy;

// setup() runs once when the Arduino starts.
void setup()
{
  xy.begin();  // Prepare the X and Y output pins.
}

// loop() runs over and over again.
void loop()
{
  // Keep drawing the box so it stays visible on the oscilloscope.
  xy.drawBox();
}
