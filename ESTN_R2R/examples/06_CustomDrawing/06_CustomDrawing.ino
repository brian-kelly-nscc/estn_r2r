// Build a picture using individual lines and points.
#include <ESTN_R2R.h>

ESTN_R2R xy;

void setup()
{
  xy.begin();

  // Time spent at each drawing point, in microseconds.
  // 120 is the library default. Try small changes and observe the scope.
  xy.setPointDelay(120);
}

void loop()
{
  // Coordinates run from 0 to 15 on each axis.
  // drawLine(startX, startY, endX, endY)

  // Bottom edge: from (2, 2) to (13, 2).
  xy.drawLine(2, 2, 13, 2);

  // Right edge: from (13, 2) to the top at (8, 14).
  xy.drawLine(13, 2, 8, 14);

  // Left edge: from the top back to (2, 2).
  xy.drawLine(8, 14, 2, 2);

  // Add a point inside the triangle.
  xy.drawPoint(8, 7);
}
