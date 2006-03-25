#ifndef PLANET_H
#define PLANET_H

#include <string>

#include "body.h"

class Ephemeris;

class Planet
{
 public:
    static body parseBodyName(char *name);

    Planet(const double jd, const body this_body);
    ~Planet();

    void calcHeliocentricEquatorial();
    void calcHeliocentricEquatorial(const bool relativeToSun);

    void PlanetocentricToXYZ(double &X, double &Y, double &Z,
			     const double lat, const double lon, 
			     const double rad);

    void XYZToPlanetocentric(const double X, const double Y, const double Z,
			     double &lat, double &lon);

    void XYZToPlanetocentric(const double X, const double Y, const double Z,
			     double &lat, double &lon, double &rad);

    void getPosition(double &X, double &Y, double &Z) const;

    void getBodyNorth(double &X, double &Y, double &Z) const;
    void getOrbitalNorth(double &X, double &Y, double &Z) const;
    void getGalacticNorth(double &X, double &Y, double &Z) const;

    body Index() const { return(index_); };
    body Primary() const { return(primary_); };

    double Flattening() const { return(flattening_); };
    int Flipped() const { return(flipped_); };
    double Period() const { return(period_); };
    double Radius() const { return(radius_); };

 private:
    body index_;

    Ephemeris *ephem_;
    bool ephemerisLow_;

    double julianDay_;
    double T2000_;            // Julian centuries from 2000
    double d2000_;            // days from 2000

    body primary_;

    double alpha0_;          // right ascension of the north pole
    double delta0_;          // declination of the north pole

    double nullMeridian_;    // orientation of zero longitude
    double nullMeridian0_;   // orientation of zero longitude at time 0
    double wdot_;            // rotation rate

    double rot_[3][3];          // rotation matrix
    double invRot_[3][3];      // inverse of e
    bool needRotationMatrix_; 

    double period_;           // orbital period, in days
    double radius_;           // equatorial radius, km
    double flattening_;       // (Re - Rp)/Re

    /* 
       flipped = 1 if planet's longitude increases to the east
       (e.g. earth), -1 if planet's longitude increases to the west
       (e.g. Mars),
    */
    int flipped_; 

    double X_, Y_, Z_;      // Position vector

    void CreateRotationMatrix();
};

#endif
