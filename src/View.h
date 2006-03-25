#ifndef VIEW_H
#define VIEW_H

class View
{
public:
    // Projection reference point (observer location)
    // View reference point (planet center)
    // display dimensions
    View(const double PRPx, const double PRPy, const double PRPz, 
	 const double VRPx, const double VRPy, const double VRPz, 
	 const double VUPx, const double VUPy, const double VUPz, 
	 const double dpp);

    ~View();

    // Rotate the point in the viewing coordinate system to heliocentric XYZ.
    void RotateToXYZ(const double X, const double Y, const double Z, 
		     double &newX, double &newY, double &newZ);

    // Rotate the point in heliocentric XYZ to the viewing coordinate system.
    void RotateToViewCoordinates(const double X, const double Y, 
				 const double Z, 
				 double &newX, double &newY, 
				 double &newZ) const;

    // Heliocentric equatorial to pixel offsets from the target planet
    // center
    void XYZToPixel(const double rotate_angle, 
		    const double X, const double Y, const double Z, 
		    double &newX, double &newY, double &newZ) const;

    void PixelToViewCoordinates(const double rotate_angle, 
				const double X, const double Y,
				double &newX, double &newY, double &newZ);

private:

    double distPerPixel;        // Astronomical units per pixel
    double distToPlane;         // distance from observer to target planet

    double rotate[3][3];
    double inv_rotate[3][3];
    double translate[3];
};

#endif
