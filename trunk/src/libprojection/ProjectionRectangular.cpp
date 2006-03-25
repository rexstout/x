#include <cmath>
using namespace std;

#include "ProjectionRectangular.h"
#include "xpUtil.h"

ProjectionRectangular::ProjectionRectangular(const int f, const int w, const int h) 
    : ProjectionBase (f, w, h)  // call the Projection constructor
{
    startLon_ = -M_PI + centerLon_;
    startLat_ = M_PI_2;

    const double map_width = TWO_PI;
    const double map_height = M_PI;

    delLat_ = map_height/height_;
    delLon_ = map_width/width_;
}

bool
ProjectionRectangular::pixelToSpherical(const double x, const double y, 
                                        double &lon, double &lat)
{
    lon = (x + 0.5) * delLon_ + startLon_;
    lat = startLat_ - (y + 0.5) * delLat_;
    return(true);
}

bool
ProjectionRectangular::sphericalToPixel(double lon, double lat,
                                        double &x, double &y) const
{
    if (lon > M_PI) lon -= TWO_PI;
    else if (lon < -M_PI) lon += TWO_PI;
          
    x = (lon - startLon_)/delLon_;

    if (x >= width_) x -= width_;
    else if (x < 0) x += width_;

    y = (startLat_ - lat)/delLat_;

    if (y >= height_) y = height_ - 1;
    
    return(true);
}

