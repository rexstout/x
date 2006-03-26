#include <cmath>
using namespace std;

#include "libdisplay/libdisplay.h"

void
drawSun(DisplayBase *display, const double X, const double Y, 
	const double R, const unsigned char *color)
{
    const int height = display->Height();
    const int width = display->Width();
    for (int j = 0; j < height; j++)
    {
        const double jdist = Y - j;      // decreases downward
	for (int i = 0; i < width; i++)
	{
            const double idist = i - X;  // increases to the right
	    const double dist = sqrt(idist * idist + jdist * jdist) / R;
	    if (dist <= 1)
		display->setPixel(i, j, color);
	    else if (dist < 28)
	    {
		const double angle = atan2(jdist, idist);
		const double brightness = 0.05 * (exp((1-dist)/5) 
						  * (19 + cos(12 * angle)));
		
		display->setPixel(i, j, color, brightness);
	    }
	}
    }
}
