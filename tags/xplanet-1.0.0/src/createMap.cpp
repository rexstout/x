#include <cmath>
#include <map>
#include <sstream>
#include <string>
using namespace std;

#include "body.h"
#include "findFile.h"
#include "Map.h"
#include "PlanetProperties.h"
#include "Ring.h"
#include "xpUtil.h"

#include "libimage/Image.h"
#include "libplanet/Planet.h"

static void
loadRGB(Image *&image, const unsigned char *&rgb, string &imageFile, 
	const string &name, const int imageWidth, const int imageHeight)
{
    bool foundFile = findFile(imageFile, "images");
    if (foundFile) 
    {
	image = new Image;
	foundFile = image->Read(imageFile.c_str());
    }
    
    if (foundFile)
    {
	if ((image->Width() != imageWidth)
	    || (image->Height() != imageHeight))
	{
	    stringstream errStr;
	    errStr << "Warning: resizing " << name
		   << " map\nFor better performance, all image maps should "
		   << "be the same size as the day map\n";
	    xpWarn(errStr.str(), __FILE__, __LINE__);
	    image->Resize(imageWidth, imageHeight);
	}
	rgb = image->getRGBData();
    }
    else
    {
	stringstream errStr;
	errStr << "Can't load map file " << imageFile << "\n";
	xpWarn(errStr.str(), __FILE__, __LINE__);
    }
}

Map *
createMap(const double sLat, const double sLon, 
	  const double obsLat, const double obsLon, 
	  const int width, const int height, 
	  const double pR,
	  Planet *planet, Ring *ring, 
	  map<double, Planet *> &planetsFromSunMap,
	  PlanetProperties *planetProperties)
{
    Map *m = NULL;

    string imageFile(planetProperties->DayMap());
    bool foundFile = findFile(imageFile, "images");
    if (!foundFile)
    {
	string errMsg("Can't find map file ");
	errMsg += imageFile;
	errMsg += "\n";
	xpWarn(errMsg, __FILE__, __LINE__);
    }

    Image *day = new Image;
    foundFile = day->Read(imageFile.c_str());
    if (!foundFile)
    {
	string errMsg("Can't load map file ");
	errMsg += imageFile;
	errMsg += "\n";
	xpWarn(errMsg, __FILE__, __LINE__);
    }

    // If the day map isn't found, assume the other maps won't be
    // found either
    if (foundFile)
    {
	int imageWidth = day->Width();
	int imageHeight = day->Height();

	const unsigned char *dayRGB = day->getRGBData();

	Image *night = NULL;
	const unsigned char *nightRGB = NULL;
	Image *cloud = NULL;
	const unsigned char *cloudRGB = NULL;
	Image *specular = NULL;
	const unsigned char *specularRGB = NULL;

	imageFile = planetProperties->NightMap();
	if (!imageFile.empty() && planetProperties->Shade() < 1) 
	    loadRGB(night, nightRGB, imageFile, "night",
		    imageWidth, imageHeight);
	
	imageFile = planetProperties->SpecularMap();
	if (!imageFile.empty())
	    loadRGB(specular, specularRGB, imageFile, "specular",
		    imageWidth, imageHeight);
	
	imageFile = planetProperties->CloudMap();
	if (!imageFile.empty())
	    loadRGB(cloud, cloudRGB, imageFile, "cloud", 
		    imageWidth, imageHeight);
	
	m = new Map(imageWidth, imageHeight, 
		    sLat, sLon, obsLat, obsLon, 
		    dayRGB, nightRGB, specularRGB, cloudRGB, 
		    planet, planetProperties, ring, planetsFromSunMap);
	
	delete night;
	delete cloud;
	delete specular;

	// If the map dimensions are each a power of two, the map size
	// will be reduced to get rid of high-frequency noise
	const double log2 = log(2.0);
	double e = log((double) imageWidth) / log2;

	double remainder = fabs(e - floor(e+0.5));
	if (remainder < 1e-3)
	    e = log((double) imageHeight) / log2;
	
	remainder = fabs(e - floor(e+0.5));
	if (remainder < 1e-3) 
	{
	    // optimal size for image is about 4*pR x 2*pR
	    const double ratio = day->Height()/pR;
	    const int factor = (int) (log(ratio)/log2) - 1;
	    m->Reduce(factor);
	}
    }
    else
    {
	int xsize = (int) (pR*4);
	if (xsize < 128) xsize = 128;
	if (xsize > width) xsize = width;
	int ysize = xsize / 2;
	m = new Map(xsize, ysize, sLat, sLon, 
		    planet, planetProperties,
		    ring, planetsFromSunMap);
    }

    delete day;

    return(m);
}    
