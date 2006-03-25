#include <cmath>
using namespace std;

#include "Options.h"
#include "ProjectionMercator.h"
#include "xpUtil.h"

ProjectionMercator::ProjectionMercator(const int f, const int w, const int h) 
    : ProjectionBase(f, w, h)
{
}

bool
ProjectionMercator::pixelToSpherical(const double x, const double y, 
                                     double &lon, double &lat)
{
    const double X = ((x - width_/2)) * TWO_PI / width_;
    const double Y = ((height_/2 - y)) * 1.5 * M_PI / height_;

    lat = atan(sinh(Y));
    lon = X;
  
    if (rotate_) RotateXYZ(lat, lon);

    if (lon > M_PI) lon -= TWO_PI;
    else if (lon < -M_PI) lon += TWO_PI;

    return(true);
}

bool
ProjectionMercator::sphericalToPixel(double lon, double lat,
				     double &x, double &y) const
{
    if (rotate_) RotateZYX(lat, lon);
  
    double X = lon;
    const double Y = log(tan(M_PI/4 + lat/2));

    if (X >= M_PI) X -= TWO_PI;
    else if (X < -M_PI) X += TWO_PI;

    x = ((width_ * X / TWO_PI) + width_ / 2);
    y = (height_ / 2 - (height_ * Y / (1.5 * M_PI)));
  
    if (y < 0 || y >= height_) return(false);
    return(true);
}
