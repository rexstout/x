#ifndef RING_H
#define RING_H

class Planet;
class View;

class Ring
{
 public:
    Ring(const double inner_radius, const double outer_radius, 
	 const double planet_radius, 
	 const double *ring_brightness, const int num_bright,
	 const double *ring_transparency, const int num_trans,
	 const double sunlon, const double sunlat,
	 const double shade, 
	 Planet *p, View *view);

    ~Ring();

    // Compute the distance of the edge of planet's shadow on the
    // rings as a function of longitude.
    void buildShadowRadiusTable();

    // get the radius of the ring shadowing the specified location on
    // the planet
    double getShadowRadius(const double lat, const double lon);

    // get the brightness on the lit side
    double getBrightness(const double lon, const double r);

    // get the brightness on the dark side
    double getBrightness(const double lon, const double r, const double t);

    double getTransparency(const double r);

    // set the size of each pixel, used to window average the ring
    // brightness/transparency
    void setDistPerPixel(const double d);

    double getOuterRadius() const { return(r_out); };

 private:
    // check if the specified part of the ring is in shadow
    bool isInShadow(const double lon, const double r);

    double r_out;  // outer ring radius, units of planetary radii
    double dr_b;   // resolution of brightness grid
    double dr_t;   // resolution of transparency grid

    int num_t;
    double *radius_t;
    double *transparency;
    int window_t;  // each pixel contains this many transparency points

    int num_b;
    double *radius_b;
    double *brightness;
    int window_b;  // each pixel contains this many brightness points

    int num_s;
    double *shadow_cosangle;
    double *shadow_radius;

    double shade_;
    double sun_lat, sun_lon;
    double sun_x, sun_y, sun_z;

    Planet *planet;
    View *sun_view;  // View coordinate system centered on the sun

    // get the outer radius of the shadow of the planet on the rings
    double getShadowRadius(const double x);

    // get a window average of array
    double getValue(const double *array, const int size, const int window,
                    const double dr, const double r);

    // get a window average of array, accounts for shadowing by the planet
    double getValue(const double *array, const int size, const int window,
                    const double dr, const double r, const double lon);
};

#endif
