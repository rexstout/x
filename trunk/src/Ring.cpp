#include <cstdlib>
#include <cmath>
using namespace std;

#include "Options.h"
#include "Ring.h"
#include "View.h"
#include "xpUtil.h"

#include "libplanet/Planet.h"

Ring::Ring(const double inner_radius, const double outer_radius, 
	   const double planet_radius, 
	   const double *ring_brightness, const int num_bright,
	   const double *ring_transparency, const int num_trans,
	   const double sunlon, const double sunlat,
	   const double shade, 
	   Planet *p, View *view) : shade_(shade)
{
    r_out = outer_radius/planet_radius;
    dr_b = (outer_radius - inner_radius) / (num_bright * planet_radius);
    dr_t = (outer_radius - inner_radius) / (num_trans * planet_radius);

    const int innerPadding = 100;
    const int outerPadding = 20;
    num_b = outerPadding + num_bright + innerPadding;
    num_t = outerPadding + num_trans + innerPadding;

    // brightness and transparency arrays are from the outside in
    radius_b = new double[num_b];
    for (int i = 0; i < num_b; i++) 
        radius_b[i] = r_out - i * dr_b;

    brightness = new double[num_b];
    // drop the brightness to 0 at the outer radius
    for (int i = 0; i < outerPadding; i++)
    {
	double weight = ((double) i) / (outerPadding - 1);
	brightness[i] = weight * ring_brightness[0];
    }

    for (int i = 0; i < num_bright; i++) 
        brightness[i + outerPadding] = ring_brightness[i];

    for (int i = 0; i < innerPadding; i++)
        brightness[outerPadding + num_bright + i] = ring_brightness[num_bright-1];

    const double cos_sun_lat = cos(sunlat);
    for (int i = 0; i < num_b; i++)
	brightness[i] *= cos_sun_lat;

    radius_t = new double[num_t];
    for (int i = 0; i < num_t; i++) 
        radius_t[i] = r_out - i * dr_t;

    transparency = new double[num_t];
    // bring the transparency up to 1 at the outer radius
    for (int i = 0; i < outerPadding; i++)
    {
	double weight = ((double) i) / (outerPadding - 1);
	transparency[i] = (1 - (1 - ring_transparency[0]) * weight);
    }

    for (int i = 0; i < num_trans; i++) 
        transparency[i + outerPadding] = ring_transparency[i];

    // bring the transparency up to 1 at the inner radius
    for (int i = 0; i < innerPadding; i++)
    {
        double weight = 1 - ((double) i) / (innerPadding - 1);
        transparency[outerPadding + num_trans + i] = (1 - (1-ring_transparency[num_trans-1]) 
						    * weight);
    }

    planet = p;

    sun_lon = sunlon;
    sun_lat = sunlat;

    sun_x = cos(sun_lat) * cos(sun_lon);
    sun_y = cos(sun_lat) * sin(sun_lon);
    sun_z = sin(sun_lat);

    sun_view = view;

    num_s = 180;
    shadow_cosangle = new double[num_s];
    shadow_radius = new double[num_s];
}

Ring::~Ring()
{
    delete [] radius_b;
    delete [] brightness;

    delete [] radius_t;
    delete [] transparency;

    delete [] shadow_cosangle;
    delete [] shadow_radius;
}

// Compute the distance of the edge of planet's shadow on the rings as
// a function of longitude.
void
Ring::buildShadowRadiusTable()
{
    int i = 0, start_index = 0;
    double d_angle = M_PI/num_s;
    for (double angle = M_PI; angle > 0; angle -= d_angle)
    {
        shadow_cosangle[i] = cos(angle);
        shadow_radius[i] = 0;
        for (int j = start_index; j < num_t; j++)
        {
            if (isInShadow(sun_lon + angle, radius_t[j]))
            {
                shadow_radius[i] = radius_t[j];
                start_index = j;
                break;
            }
        }
        if (shadow_radius[i] == 0)
        {
            num_s = i;
            shadow_radius[i] = radius_t[num_t - 1];
            break;
        }

	i++;
    }
}

/*
  If the part of the ring at the specified longitude and radius
  isn't visible from the Sun, it's in shadow.
*/
bool
Ring::isInShadow(const double lon, double r)
{
    double X, Y, Z;
    planet->PlanetocentricToXYZ(X, Y, Z, 0, lon, r);
    sun_view->XYZToPixel(0, X, Y, Z, X, Y, Z);

    double distsq = X*X + Y*Y;

    if (distsq < 1) return(true);
    return(false);
}

/*
  Given a subsolar point and a location on the planet surface, check
  if the surface location can be in shadow by the rings, and if so,
  return the ring radius.
*/
double
Ring::getShadowRadius(const double lat, const double lon)
{
    // If this point is on the same side of the rings as the sun,
    // there's no shadow.
    if(sun_lat * lat >= 0) return(-1);

    const double x = cos(lat) * cos(lon);
    const double y = cos(lat) * sin(lon);
    const double z = sin(lat);

    const double dist = z/sun_z;

    const double dx = x - sun_x * dist;
    const double dy = y - sun_y * dist;
    
    return(sqrt(dx * dx + dy * dy));
}

/*
  Given a cos(longitude), return the radius of the outermost point of the
  planet's shadow on the ring.
*/
double Ring::getShadowRadius(const double x) 
{
    for (int i = 0; i < num_s; i++)
    {
        if (shadow_cosangle[i] > x) 
        {
            double frac = ((x - shadow_cosangle[i-1])
                           /(shadow_cosangle[i] - shadow_cosangle[i-1]));
            double returnval = (shadow_radius[i-1] 
                                + frac * (shadow_radius[i] 
                                          - shadow_radius[i-1]));
            return(returnval);
        }
    }
    return(0);
}

double 
Ring::getBrightness(const double lon, const double r)
{
    return(getValue(brightness, num_b, window_b, dr_b, r, lon));
}

double 
Ring::getBrightness(const double lon, const double r, const double t)
{
    double returnval;
    if (t == 1.0) 
    {
        returnval = 0;
    }
    else 
    {
        returnval = getValue(transparency, num_t, window_t, dr_t, r, lon);
    }
    return(returnval);
}

double
Ring::getTransparency(const double r)
{
    return(getValue(transparency, num_t, window_t, dr_t, r));
}

double
Ring::getValue(const double *array, const int size, const int window,
               const double dr, const double r)
{
    int i = (int) ((r_out - r)/dr);

    if (i < 0 || i >= size) return(-1.0);

    int j1 = i - window;
    int j2 = i + window;
    if (j1 < 0) j1 = 0;
    if (j2 >= size) j2 = size - 1;

    double sum = 0;
    for (int j = j1; j < j2; j++) sum += array[j];
    sum /= (j2 - j1);

    return(sum);
}

double
Ring::getValue(const double *array, const int size, const int window,
               const double dr, const double r, const double lon)
{
    double cos_lon = cos(lon-sun_lon);
    if (cos_lon > -0.45) return(getValue(array, size, window, dr, r));
    
    int i = (int) ((r_out - r)/dr);

    if (i < 0 || i >= size) return(-1.0);

    int j1 = i - window;
    int j2 = i + window;
    const int interval = j2 - j1;

    if (j1 < 0) j1 = 0;
    if (j2 >= size) j2 = size - 1;

    double shadow_rad = getShadowRadius(cos_lon);

    double r0 = r;
    double sum = 0;
    for (int j = j1; j < j2; j++) 
    {
        if (r0 < shadow_rad)
            sum += (shade_ * array[j]);
        else
            sum += array[j];
        r0 += dr;
    }
    sum /= interval;

    return(sum);
}

// units for dist_per_pixel is planetary radii
void
Ring::setDistPerPixel(const double dist_per_pixel)
{
    window_b = (int) (dist_per_pixel / dr_b + 0.5);
    window_t = (int) (dist_per_pixel / dr_t + 0.5);

    window_b = window_b/2 + 1;
    window_t = window_t/2 + 1;
}

