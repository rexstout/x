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

    separation_ = acos(ndot(tX_ - oX_, tY_ - oY_, tZ_ - oZ_,
			    sX_ - oX_, sY_ - oY_, sZ_ - oZ_));
    angle_ =  acos(ndot(oX_ - tX_, oY_ - tY_, oZ_ - tZ_, 
			sX_ - tX_, sY_ - tY_, sZ_ - tZ_));

    printf("Observer: (%14.6f, %14.6f, %14.6f)\n", oX_, oY_, oZ_);
    printf("Target 1: (%14.6f, %14.6f, %14.6f)\n", tX_, tY_, tZ_);
    printf("Target 2: (%14.6f, %14.6f, %14.6f)\n", sX_, sY_, sZ_);
    printf("Separation = %14.6f, angle = %14.6f\n", separation_/deg_to_rad, 
	   angle_/deg_to_rad);
}

Separation::~Separation()
{
    delete view_;
}

void
Separation::getOrigin(double &oX, double &oY, double &oZ)
{
    oX = oX_;
    oY = oY_;
    oZ = oZ_;
}

// Assume observer is at (0, 0, z0), target 1 is at (0, 0, 0), and
// target 2 is at (0, y2, z2).  Supply an angle, and calculate the new
// observer location.
void
Separation::setAngle(const double angle)
{
}
