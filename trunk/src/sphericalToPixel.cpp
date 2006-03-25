#include <cmath>
#include <cstdlib>
using namespace std;

#include "Options.h"
#include "View.h"
#include "xpUtil.h"

#include "libplanet/Planet.h"
#include "libprojection/ProjectionBase.h"

bool
sphericalToPixel(const double lat, const double lon, const double rad, 
		 double &X, double &Y, double &Z, Planet *planet, 
		 View *view, ProjectionBase *projection)
{
    bool returnval = false;

    Options *options = Options::getInstance();

    if (view != NULL)
    {
	planet->PlanetocentricToXYZ(X, Y, Z, lat, lon, rad);

	view->XYZToPixel(options->getRotate(), 
			 X, Y, Z, X, Y, Z);
	X += options->getCenterX();
	Y += options->getCenterY();

	returnval = true;
    }
    else if (projection != NULL)
    {
	returnval = projection->sphericalToPixel(lon, lat, X, Y);
	Z = 0;
    }
    else
    {
	xpExit("Both view and projection are NULL!\n", __FILE__, __LINE__);
    }

    return(returnval);
}
