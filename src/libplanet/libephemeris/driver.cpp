#include <cmath>
#include <cstdio>
using namespace std;

#include "body.h"

extern void
ephemeris(const body b, const double tjd, 
          double &Px, double &Py, double &Pz);

int
main(int argc, char **argv)
{
    body b[9] = {
	MERCURY, 
	VENUS, 
	EARTH,
	MARS, 
	JUPITER,
	SATURN,
	URANUS,
	NEPTUNE, 
	PLUTO 
    };

    double tjd[3];
    tjd[0] = 2196035.00000000000;     // 01 Jun 1300
    tjd[1] = 2451697.00000000000;     // 01 Jun 2000
    tjd[2] = 2707366.00000000000;     // 01 Jun 2700

    for (int j = 0; j < 3; j++)
    {
	printf("JD: %14.5f\n", tjd[j]);
	for (int i = 0; i < 9; i++)
	{
	    double Px, Py, Pz;
	    ephemeris(b[i], tjd[j], Px, Py, Pz);
	    
	    printf("x :%16.12f  y :%16.12f  z :%16.12f\n", Px, Py, Pz);
	    
	    double Ex, Ey, Ez;
	    ephemeris(EARTH, tjd[j], Ex, Ey, Ez);
	    
	    Px -= Ex;
	    Py -= Ey;
	    Pz -= Ez;

	    double dist = sqrt(Px*Px + Py*Py + Pz*Pz);
	    double light_time = dist * 149597870660. / 299792458;
	    light_time /= 86400;
	    ephemeris(b[i], tjd[j] - light_time, Px, Py, Pz);

	    Px -= Ex;
	    Py -= Ey;
	    Pz -= Ez;

	    printf("x :%16.12f  y :%16.12f  z :%16.12f\n\n", Px, Py, Pz);
	}
    }
}
