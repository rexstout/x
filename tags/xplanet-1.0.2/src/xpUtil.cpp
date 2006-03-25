#include <cmath>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

#include <unistd.h>
extern char **environ;

#include "config.h"

#include "Options.h"
#include "xpUtil.h"

void 
xpExit(const string &message, const char *file, const int line)
{
    cerr << "Error: " << message;
    cerr << "Exiting from " << file << " at line " << line << endl;
    exit(EXIT_FAILURE);
}

void 
xpWarn(const string &message, const char *file, const int line)
{
    Options *options = Options::getInstance();
    if (options->Verbosity() >= 0) 
    {
        cerr << "Warning: " << message;
        if (options->Verbosity() > 0) 
            cerr << "In " << file << " at line " << line << endl << endl;
        cerr.flush();
    }
}

void 
xpMsg(const string &message, const char *file, const int line)
{
    Options *options = Options::getInstance();
    if (options->Verbosity() > 0) 
    {
        cout << message;
        cout.flush();
    }
}

void
removeFromEnvironment(const char *name)
{
#ifdef HAVE_UNSETENV
    unsetenv(name);
#else
    string badname = name;
    badname += "=";

    // I found this useful code on groups.google.com.  It's based on
    // sudo's code, where the environment is cleaned up before
    // executing anything.
    char **cur, **move;
    for (cur = environ; *cur; cur++) {
        if (strncmp(*cur, badname.c_str(), badname.length()) == 0)
        {
            /* Found variable; move subsequent variables over it */
            for (move = cur; *move; move++)
                *move = *(move + 1);
            cur--;
        }
    }

#endif
}

void
unlinkFile(const char *name)
{
    if (unlink(name) == -1)
    {
        ostringstream errStr;
        errStr << "Can't remove " << name << "\n";
        xpWarn(errStr.str(), __FILE__, __LINE__);
    }
}

void
cross(const double a[3], const double b[3], double c[3])
{
    c[0] = a[1]*b[2] - b[1]*a[2];
    c[1] = a[2]*b[0] - b[2]*a[0];
    c[2] = a[0]*b[1] - b[0]*a[1];
}

double 
dot(const double A0, const double A1, const double A2, 
    const double B0, const double B1, const double B2)
{
    return(A0 * B0 + A1 * B1 + A2 * B2);
}

double 
ndot(const double A0, const double A1, const double A2, 
     const double B0, const double B1, const double B2)
{
    const double len_a = sqrt(dot(A0, A1, A2, A0, A1, A2));
    const double len_b = sqrt(dot(B0, B1, B2, B0, B1, B2));
    return(dot(A0, A1, A2, B0, B1, B2) / (len_a * len_b));
}

double 
dot(const double a[3], const double b[3])
{
    return(a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

double
ndot(const double a[3], const double b[3])
{
    const double len_a = dot(a, a);
    const double len_b = dot(b, b);
    return(dot(a, b) / sqrt(len_a * len_b));
}

void
invertMatrix(double in[3][3], double out[3][3])
{
    double a1 = in[0][0];
    double a2 = in[0][1];
    double a3 = in[0][2];

    double b1 = in[1][0];
    double b2 = in[1][1];
    double b3 = in[1][2];

    double c1 = in[2][0];
    double c2 = in[2][1];
    double c3 = in[2][2];

    double det = (a1*(b2*c3 - b3*c2) + a2*(b3*c1 - b1 * c3) 
                  + a3*(b1*c2 - b2*c1));
    
    out[0][0] = (b2*c3 - b3*c2)/det;
    out[0][1] = (a3*c2 - a2*c3)/det;
    out[0][2] = (a2*b3 - a3*b2)/det;

    out[1][0] = (b3*c1 - b1*c3)/det;
    out[1][1] = (a1*c3 - a3*c1)/det;
    out[1][2] = (a3*b1 - a1*b3)/det;

    out[2][0] = (b1*c2 - b2*c1)/det;
    out[2][1] = (a2*c1 - a1*c2)/det;
    out[2][2] = (a1*b2 - a2*b1)/det;
}

void
RADecToXYZ(double RA, double Dec, double &X, double &Y, double &Z)
{
    double dist = 1e6;
    
    RA *= 15;
    
    X = dist * cos(Dec) * cos(RA);
    Y = dist * cos(Dec) * sin(RA);
    Z = dist * sin(Dec);
}

void
fromJulian(double jd, int &year, int &month, int &day, 
           int &hour, int &min, double &sec)
{
    jd += 0.5;
    int Z = (int) jd;
    double F = jd - Z;
    
    int A = Z;
    if (Z >= 2291161)
    {
        int alpha = (int) ((Z - 1867216.25)/36524.25);
        A = Z + 1 + alpha - alpha/4;
    }

    int B = A + 1524;
    int C = (int) floor((B - 122.1) / 365.25);
    int D = (int) floor(365.25 * C);
    int E = (int) floor((B - D)/30.6001);

    double dday = B - D - (int) floor(30.6001 * E) + F;

    day = (int) floor(dday);

    month = E - 13;
    if (E < 14) month = E - 1;

    year = C - 4715;
    if (month > 2) year--;

    double dhour = 24 * (dday - day);
    hour = (int) floor(dhour);

    double dmin = 60 * (dhour - hour);
    min = (int) floor(dmin);
    sec = 60 * (dmin - min);
}

string
fromJulian(double date)
{
    char timeString[16];
    int year, month, day, hour, min;
    double sec;
    
    fromJulian(date, year, month, day, hour, min, sec);
    snprintf(timeString, 16, 
             "%4.4d%2.2d%2.2d.%2.2d%2.2d%2.2d",
             year, month, day, 
             hour, min, (int) floor(sec));

    return(string(timeString));
}

time_t
get_tv_sec(double jd)
{
    int year, month, day, hour, min;
    double sec;
    fromJulian(jd, year, month, day, hour, min, sec);

    tm tm_struct = { (int) floor(sec + 0.5), min, hour, day,
                     month - 1, year - 1900, 0, 0, -1 };

#ifdef HAVE_TIMEGM
    time_t returnval = timegm(&tm_struct);
#else
    string tz_save = "";
    char *get_tz = getenv("TZ");
    if (get_tz != NULL)
    {
        tz_save = "TZ=";
        tz_save += get_tz;
    }
    putenv("TZ=UTC");
    tzset();

    time_t returnval = mktime(&tm_struct);

    if (tz_save.empty()) 
        removeFromEnvironment("TZ"); 
    else
        putenv((char *) tz_save.c_str()); 
    tzset();
#endif
    return(returnval);
}

double 
toJulian(int year, int month, int day, int hour, int min, int sec)
{
    // Gregorian calendar (after 1582 Oct 15)
    const bool gregorian = (year > 1582 
                            || (year == 1582 && month > 10)
                            || (year == 1582 && month == 10 && day >= 15));

    if(month < 3) 
    {
        year -= 1;
        month += 12;
    }      
  
    int a = year/100;
    int b = 0;
    if (gregorian) b = 2 - a + a/4;

    int c = (int) floor(365.25 * (year + 4716));
    int d = (int) floor(30.6001 * (month + 1));
    double e = day + ((sec/60. + min) / 60. + hour) / 24.;

    double jd = b + c + d + e - 1524.5;

    return(jd);
}

// find the difference between universal and ephemeris time 
// (delT = ET - UT)
double 
delT(const double jd)
{
#if 0
    // From McCarthy & Babcock (1986)
    const double j1800 = 2378496.5;
    const double t = (jd - j1800)/36525;
    const double delT = 5.156 + 13.3066 * (t - 0.19) * (t - 0.19);
#else
    // Valid from 1825 to 2000, Montenbruck & Pfelger (2000), p 188
    const double T = (jd - 2451545)/36525;
    const int i = (int) floor(T/0.25);

    const double c[7][4] = {
        { 10.4, -80.8,  413.9,  -572.3 },
        {  6.6,  46.3, -358.4,    18.8 },
        { -3.9, -10.8, -166.2,   867.4 },
        { -2.6, 114.1,  327.5, -1467.4 },
        { 24.2,  -6.3,   -8.2,   483.4 },
        { 29.3,  32.5,   -3.8,   550.7 },
        { 45.3, 130.5, -570.5,  1516.7 }
    };

    double t = T - i * 0.25;

    int ii = i + 7;
    if (ii < 0) 
    {
        t = 0;
        ii = 0;
    }
    else if (ii > 6) 
    {
        ii = 6;
        t = 0.25;
    }

    const double delT = c[ii][0] + t * (c[ii][1] 
                                        + t * (c[ii][2] 
                                               + t * c[ii][3]));
#endif

    return(delT);
}

double 
poly(const double a1, const double a2, const double a3, const double a4,
     const double t)
{
    return(a1 + t*(a2 + t*(a3 + t*a4)));
}

void 
rotateX(double &X, double &Y, double &Z, const double theta)
{
    const double st = sin(theta);
    const double ct = cos(theta);
    const double X0 = X;
    const double Y0 = Y;
    const double Z0 = Z;

    X = X0;
    Y = Y0 * ct + Z0 * st;
    Z = Z0 * ct - Y0 * st;
}

void 
rotateZ(double &X, double &Y, double &Z, const double theta)
{
    const double st = sin(theta);
    const double ct = cos(theta);
    const double X0 = X;
    const double Y0 = Y;
    const double Z0 = Z;

    X = X0 * ct + Y0 * st;
    Y = Y0 * ct - X0 * st;
    Z = Z0;
}

/*
  x = 0 passes through 0 and 2
  y = 0 passes through 0 and 1
  
  Given a point (x, y), compute the area weighting of each pixel.

  --- ---
 | 0 | 1 |
  --- ---
 | 2 | 3 |
  --- ---
*/
void
getWeights(const double t, const double u, double weights[4])
{
    // Weights are from Numerical Recipes, 2nd Edition
    //        weight[0] = (1 - t) * u;
    //        weight[2] = (1-t) * (1-u);
    //        weight[3] = t * (1-u);
    weights[1] = t * u;
    weights[0] = u - weights[1];
    weights[2] = 1 - t - u + weights[1];
    weights[3] = t - weights[1];
}

void
calcGreatArc(const double lat1, const double lon1,
             const double lat2, const double lon2,
             double &trueCourse, double &dist)
{
    /*
     * Equations are from http://www.best.com/~williams/avform.html
     * returned trueCourse is relative to latitude north
     */
    const double sin_lat1 = sin(lat1);
    const double cos_lat1 = cos(lat1);
    const double sin_lat2 = sin(lat2);
    const double cos_lat2 = cos(lat2);
    const double dlon = lon1 - lon2;

    // Arc length between points (in radians)
    dist = acos(sin_lat1 * sin_lat2 + cos_lat1 * cos_lat2 * cos(dlon));

    // True course
    trueCourse = fmod(atan2(sin(dlon) * cos_lat2, 
                            cos_lat1 * sin_lat2 
                            - sin_lat1 * cos_lat2 * cos(dlon)), TWO_PI);
}

double
kepler(const double e, double M)
{
    double E = M = fmod(M, TWO_PI);
    double delta = 1;

    while (fabs(delta) > 1E-10)
    {
        delta = (M + e * sin(E) - E)/(1 - e * cos(E));
        E += delta;
    }
    return(E);
}

//  Precess rectangular coordinates in B1950 frame to J2000 using
//  Standish precession matrix from Lieske (1998)
void
precessB1950J2000(double &X, double &Y, double &Z)
{
    const static double p[3][3] =
        { { 0.9999256791774783, -0.0111815116768724, -0.0048590038154553 },
          { 0.0111815116959975,  0.9999374845751042, -0.0000271625775175 },
          { 0.0048590037714450, -0.0000271704492210,  0.9999881946023742 } };

    double newX = p[0][0] * X + p[0][1] * Y + p[0][2] * Z;
    double newY = p[1][0] * X + p[1][1] * Y + p[1][2] * Z;
    double newZ = p[2][0] * X + p[2][1] * Y + p[2][2] * Z;

    X = newX;
    Y = newY;
    Z = newZ;
}
