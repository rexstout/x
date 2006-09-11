#include <cmath>
#include <iostream>

#include "Separation.h"
#include "View.h"
#include "xpUtil.h"

using namespace std;

Separation::Separation(const double oX, const double oY, const double oZ,
		       const double tX, const double tY, const double tZ, 
		       const double sX, const double sY, const double sZ) 
{
    view_ = new View(tX, tY, tZ, oX, oY, oZ, sX, sY, sZ, 0, 0);
    view_->RotateToViewCoordinates(oX, oY, oZ, oX_, oY_, oZ_);
    view_->RotateToViewCoordinates(tX, tY, tZ, tX_, tY_, tZ_);
    view_->RotateToViewCoordinates(sX, sY, sZ, sX_, sY_, sZ_);

    oR_ = sqrt(oY_ * oY_ + oZ_ * oZ_);
    sR_ = sqrt(sY_ * sY_ + sZ_ * sZ_);
}

Separation::~Separation()
{
    delete view_;
}

void
Separation::getOrigin(double &oX, double &oY, double &oZ)
{
    view_->RotateToXYZ(oX_, oY_, oZ_, oX, oY, oZ);
}

// Given an angle sep, calculate the observer position
void
Separation::setSeparation(double sep)
{
    if (oR_ > sR_)
    {
	double max_sep = asin(sR_/oR_);
	if (sep > max_sep)
	{
	    sep = max_sep;
	    printf("Maximum separation is %14.6f\n", max_sep / deg_to_rad);
	}
    }

    const double alpha0 = atan2(sY_, sZ_);

#if 0
    int numPts = 20;
    for (int i = 0; i < numPts; i++)
    {
	double angle = alpha0 + i * TWO_PI / (numPts - 1.);
	printf("%d) %14.6f %14.6f\n", i, angle, calcSeparation(angle));
    }
#endif

    double x0 = alpha0;
    double xmid = alpha0 + M_PI;
    double x1 = alpha0 + TWO_PI;

    if (calcSeparation(xmid) > sep)
	x0 = xmid;
    else
	x1 = xmid;

    for (int i = 0; i < 200; i++)
    {
	xmid = 0.5 * (x1 + x0);
	if (calcSeparation(xmid) > sep)
	    x0 = xmid;
	else
	    x1 = xmid;

	if (fabs(x0 - x1) < 1e-8) break;
    }    
}

// Assume target 1 is at (0, 0, 0) and target 2 is at (0, y2, z2).
// Supply an angle, and calculate the new observer location (0, y, z)
// and the separation between the two targets.
double
Separation::calcSeparation(const double alpha)
{
    oX_ = 0;
    oY_ = oR_ * sin(alpha);
    oZ_ = oR_ * cos(alpha);

    const double t[3] = { tX_ - oX_, tY_ - oY_, tZ_ - oZ_ };
    const double s[3] = { sX_ - oX_, sY_ - oY_, sZ_ - oZ_ };

    double separation = acos(ndot(t, s));

    double c[3];
    cross(s, t, c);

    if (c[0] < 0) separation *= -1;

    return(separation);
}
