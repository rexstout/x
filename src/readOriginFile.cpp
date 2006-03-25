#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
using namespace std;

#include "findFile.h"
#include "readOriginFile.h"
#include "xpUtil.h"

void
readOriginFile(string filename, vector<LBRPoint> &originVector)
{
    if (!findFile(filename, "origin"))
    {
	stringstream errStr;
	errStr << "Can't open origin file " << filename << "\n";
	xpExit(errStr.str(), __FILE__, __LINE__);
    }

    ifstream infile(filename.c_str());
    char line[256];
    while(infile.getline(line, 256))
    {
	long int yyyymmdd, hhmmss;
	double lat, lon, r;
	if (line[0] == '#') continue;

	sscanf(line, "%ld.%ld %lf %lf %lf", &yyyymmdd, &hhmmss, &r, &lat, &lon);
	int yyyymm = yyyymmdd / 100;
	int year = yyyymm/100;
	int month = abs(yyyymm - year * 100);
	int day = abs((int) yyyymmdd - yyyymm * 100);
	
	int hhmm = hhmmss / 100;
	int hour = hhmm / 100;
	int min = hhmm - hour * 100;
	int sec = hhmmss - hhmm * 100;
	
	const double julian_day = toJulian(year, month, day, hour, min, sec);

	lat *= deg_to_rad;
	lon *= deg_to_rad;

	LBRPoint p = { julian_day, lat, lon, r };

	originVector.push_back(p);
    }
}
