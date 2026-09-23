// Choose a picture by sending a command from Serial Monitor.
#include <ESTN_R2R.h>

ESTN_R2R xy;

void setup()
{
  xy.begin();

  // Set Serial Monitor to the same baud rate as this number.
  Serial.begin(115200);

  // Show the available commands in Serial Monitor.
  xy.printHelp(Serial);
}

void loop()
{
  // Read commands and redraw the selected picture.
  // Try B for box, S for star, or a digit from 0 to 9.
  xy.updateSerial(Serial);
}
