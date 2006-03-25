#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

#include "findFile.h"
#include "Options.h"
#include "View.h"
#include "xpUtil.h"
#include "libdisplay/libdisplay.h"

void
drawStars(DisplayBase *display, View *view)
{
    Options *options = Options::getInstance();

    string starMap = options->getStarMap();
    if (!findFile(starMap, "stars"))
    {
	stringstream errMsg;
	errMsg << "Can't open star map " << starMap << endl;
	xpWarn(errMsg.str(), __FILE__, __LINE__);
	return;
    }

    const int width = display->Width();
    const int height = display->Height();
    const int area = width * height;

    bool *starPresent = new bool [area];
    double *magnitude = new double [area];
    for (int i = 0; i < area; i++) 
    {
	starPresent[i] = false;
	magnitude[i] = 0;
    }

    ifstream infile(starMap.c_str());

    char line[256];
    while(infile.getline(line, 256))
    {
	double Vmag, RA, Dec;
	sscanf(line, "%lf %lf %lf", &Vmag, &RA, &Dec);

	double dist = 1e6;

	RA *= 15;
	RA *= deg_to_rad;
	Dec *= deg_to_rad;

	const double sX = dist * cos(Dec) * cos(RA);
	const double sY = dist * cos(Dec) * sin(RA);
	const double sZ = dist * sin(Dec);

	double X, Y, Z;
	view->XYZToPixel(options->getRotate(), sX, sY, sZ, X, Y, Z);
	X += options->getCenterX();
	Y += options->getCenterY();

	if (Z < 0 
	    || X < 0 || X >= width
	    || Y < 0 || Y >= height) continue;

	int ipos[4];
	ipos[0] = ((int) floor(Y)) * width + ((int) floor(X));
	ipos[1] = ipos[0] + 1;
	ipos[2] = ipos[0] + width;
	ipos[3] = ipos[2] + 1;

	const double t = X - floor(X);
	const double u = 1 - (Y - floor(Y));

	double weight[4];
	getWeights(t, u, weight);

	for (int i = 0; i < 4; i++)
	{
	    if (ipos[i] >= area) ipos[i] = ipos[0];
	    magnitude[ipos[i]] += weight[i] * pow(10, -0.4 * Vmag);
	    starPresent[ipos[i]] = true;
	}
    }
    infile.close();

    for (int i = 0; i < area; i++)
    {
	if (starPresent[i])
	    magnitude[i] = -2.5 * log10(magnitude[i]);
    }

    // a magnitude 10 star will have a pixel brightness of 1
    const double baseMag = 10;

    for (int j = 0; j < height; j++)
    {
	int istart = j * width;
	for (int i = 0; i < width; i++)
	{
	    const double mag = magnitude[istart+i];
	    if (starPresent[istart+i])
	    {
		double brightness = pow(10, -0.4 * (mag - baseMag));

		if (brightness > 255)
		    brightness = 255;

		display->setPixel(i, j, (unsigned int) brightness);
	    }
	}
    }

    delete [] magnitude;
    delete [] starPresent;
}
