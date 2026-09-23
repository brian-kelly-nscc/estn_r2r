// Show each picture for 1.5 seconds, then move to the next one.
#include <ESTN_R2R.h>

ESTN_R2R xy;

// Change this number to choose how long each picture stays on screen.
// 1000 milliseconds = 1 second.
const unsigned long pictureDuration = 1500;

// millis() gives the time since startup in milliseconds.
// unsigned long is the number type Arduino uses for this clock.
unsigned long pictureStartTime;

void setup()
{
  xy.begin();
}

void loop()
{
  // A picture must be drawn repeatedly to stay visible.
  // Each while loop keeps drawing until the chosen time has passed.
  // We use elapsed time (now minus start), so clock rollover also works.
  // delay() would stop drawing and leave the beam sitting on one point.

  // Show the box.
  pictureStartTime = millis();
  while (millis() - pictureStartTime < pictureDuration)
  {
    xy.drawBox();
  }

  // Show the triangle.
  pictureStartTime = millis();
  while (millis() - pictureStartTime < pictureDuration)
  {
    xy.drawTriangle();
  }

  // Show the diamond.
  pictureStartTime = millis();
  while (millis() - pictureStartTime < pictureDuration)
  {
    xy.drawDiamond();
  }

  // Show the circle.
  pictureStartTime = millis();
  while (millis() - pictureStartTime < pictureDuration)
  {
    xy.drawCircle();
  }

  // Show the star.
  pictureStartTime = millis();
  while (millis() - pictureStartTime < pictureDuration)
  {
    xy.drawStar();
  }

  // Show the digit 8.
  pictureStartTime = millis();
  while (millis() - pictureStartTime < pictureDuration)
  {
    xy.drawDigit(8);
  }

  // Show the word GO.
  pictureStartTime = millis();
  while (millis() - pictureStartTime < pictureDuration)
  {
    xy.drawGO();
  }

  // At the end, loop() starts again with the box.
}
