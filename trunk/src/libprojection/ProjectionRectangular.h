#ifndef PROJECTIONRECTANGULAR_H
#define PROJECTIONRECTANGULAR_H

#include "ProjectionBase.h"

class ProjectionRectangular : public ProjectionBase
{
 public:
    ProjectionRectangular(const int f, const int w, const int h);
    bool pixelToSpherical(const double x, const double y, 
			  double &lon, double &lat);

    bool sphericalToPixel(double lon, double lat, double &x, double &y) const;

 private:
    double startLat_, startLon_;
    double delLat_, delLon_;
};

#endif
