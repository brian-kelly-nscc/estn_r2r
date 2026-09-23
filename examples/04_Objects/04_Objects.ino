// Next step: store several separate objects in one picture.
#include <ESTN_R2R.h>

ESTN_R2R xy;
ESTN_R2RFrame frame;

void setup()
{
  xy.begin();

  // An array is a list of values. Each byte stores one X/Y point.
  // packPoint(x, y) combines the two coordinates into that byte.
  const byte snakePoints[] = {
    xy.packPoint(8, 8),
    xy.packPoint(7, 8),
    xy.packPoint(6, 8),
    xy.packPoint(5, 8)
  };

  const byte foodPoints[] = {
    xy.packPoint(12, 12)
  };

  // sizeof gives the array size in bytes. Here each point is one byte,
  // so this also tells us how many points are in the list.
  const byte snakePointCount = sizeof(snakePoints);
  const byte foodPointCount = sizeof(foodPoints);

  // addObject copies the points into the frame.
  // It returns true if they fit, or false if the frame is full.
  bool snakeAdded = frame.addObject(snakePoints, snakePointCount);
  bool foodAdded = frame.addObject(foodPoints, foodPointCount);

  // If either object did not fit, remove the incomplete picture.
  if (snakeAdded == false)
  {
    frame.clear();
  }

  if (foodAdded == false)
  {
    frame.clear();
  }
}

void loop()
{
  // Draw the snake and food as separate objects.
  // Without beam blanking, faint travel traces may still be visible.
  xy.drawFrame(frame);
}
