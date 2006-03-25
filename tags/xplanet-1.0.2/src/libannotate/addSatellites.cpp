#include <cmath>
#include <cstdio>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

#include "findFile.h"
#include "keywords.h"
#include "Options.h"
#include "parse.h"
#include "PlanetProperties.h"
#include "Satellite.h"
#include "sphericalToPixel.h"
#include "View.h"
#include "xpUtil.h"

#include "drawArc.h"
#include "drawCircle.h"

#include "libannotate/libannotate.h"
#include "libplanet/Planet.h"
#include "libprojection/ProjectionBase.h"

static vector<Satellite> satelliteVector;

static void
readSatelliteFile(const char *line, Planet *planet, 
		  View *view, ProjectionBase *projection,
		  PlanetProperties *planetProperties, 
		  multimap<double, Annotation *> &annotationMap)
{
    int i = 0;
    while (isDelimiter(line[i]))
    {
	i++;
	if (static_cast<unsigned int> (i) > strlen(line)) return;
    }
    if (isEndOfLine(line[i])) return;

    Options *options = Options::getInstance();

    unsigned char color[3];
    memcpy(color, planetProperties->MarkerColor(), 3);

    int align = RIGHT;
    vector<double> altcirc;
    string font("");
    int fontSize = -1;
    string image;
    string name("");
    Satellite *satellite = NULL;
    int symbolSize = 2;
    double spacing = 0.1;
    bool syntaxError = false;
    string timezone;

    int trailType = ORBIT;
    int trailStart = 0;
    int trailEnd = 0;
    int trailInterval = 1;

    bool transparency = false;
    unsigned char transparent_pixel[3];

    while (static_cast<unsigned int> (i) < strlen(line))
    {
	char *returnString = NULL;
	int val = parse(i, line, returnString);

	switch (val)
	{
	case ALIGN:
            if (returnString == NULL) break;
            switch (returnString[0])
            {
            case 'r':
            case 'R':
                align = RIGHT;
                break;
            case 'l':
            case 'L':
                align = LEFT;
                break;
            case 'a':
            case 'A':
                align = ABOVE;
                break;
            case 'b':
            case 'B':
                align = BELOW;
                break;
            case 'c':
            case 'C':
                align = CENTER;
                break;
            default:
		xpWarn("Unrecognized option for align in satellite file\n",
		       __FILE__, __LINE__);
		syntaxError = true;
                break;
            }
	    break;
	case CIRCLE:
        {
            double angle;
            sscanf(returnString, "%lf", &angle);
            if (angle < 0) angle *= -1;
            if (angle > 90) angle = 90;
            angle = 90 - angle;
            altcirc.push_back(angle * deg_to_rad);
        }
	break;
	case COLOR:
	{
	    int r, g, b;
	    if (sscanf(returnString, "%d,%d,%d", &r, &g, &b) == 3)
	    {
		color[0] = r & 0xff;
		color[1] = g & 0xff;
		color[2] = b & 0xff;
	    }
	    else
	    {
		xpWarn("need three values for color\n", __FILE__, __LINE__);
		syntaxError = true;
	    }
	}
	break;
	case FONT:
	    font.assign(returnString);
	    break;
	case FONTSIZE:
            sscanf(returnString, "%d", &fontSize);
	    if (fontSize <= 0)
	    {
		xpWarn("fontSize must be positive.\n", __FILE__, __LINE__);
		syntaxError = true;
	    }
	    break;
	case IMAGE:
	    image.assign(returnString);
	    break;
	case LATLON:
	{
	    int id;
	    sscanf(returnString, "%d", &id);
	    vector<Satellite>::iterator ii = satelliteVector.begin();
	    while (ii != satelliteVector.end())
	    {
		if (ii->getID() == id)
		{
		    satellite = &(*ii);
		    if (name.empty()) name.assign(satellite->getName());
		    if (options->Verbosity() > 3)
		    {
			stringstream msg;
			msg << "Found satellite # " << id 
			    << " (" << satellite->getName() << ")\n";
			xpMsg(msg.str(), __FILE__, __LINE__);
		    }
		    break;
		}
		ii++;
	    }
	}
	break;
	case NAME:
	    name.assign(returnString);
	    break;
	case SPACING:
	    sscanf(returnString, "%lf", &spacing);
	    if (spacing < 0) 
	    {
		xpWarn("spacing must be positive\n", __FILE__, __LINE__);
		spacing = 0.1;
		syntaxError = true;
	    }
	    break;
        case TRAIL:
        {
            char *ptr = returnString;
            while (ptr[0] != ',') 
            {
                if (ptr[0] == '\0') 
                {
                    syntaxError = true;
                    break;
                }
                ptr++;
            }

            if (syntaxError) break;

            if (!sscanf(++ptr, "%d,%d,%d", &trailStart, &trailEnd,
                        &trailInterval) == 3)
            {
                xpWarn("Need four values for trail{}!\n", __FILE__, __LINE__);
                syntaxError = true;
            }
            else
            {
                switch (returnString[0])
                {
                case 'g':
                case 'G':
                    trailType = GROUND;
                    break;
                case 'o':
                case 'O':
                    trailType = ORBIT;
                    break;
                default:
                    xpWarn("Unknown type of orbit trail!\n", __FILE__, __LINE__);
                    syntaxError = true;
                    break;
                }
                if (trailInterval < 1) trailInterval = 1;
            }
        }
        break;
	case TRANSPARENT:
            transparency = true;

            int r, g, b;
            if (sscanf(returnString, "%d,%d,%d", &r, &g, &b) == 3)
            {
                transparent_pixel[0] = r & 0xff;
                transparent_pixel[1] = g & 0xff;
                transparent_pixel[2] = b & 0xff;
            }
            else
            {
                xpWarn("Need three values for transparency pixel!\n", __FILE__, __LINE__);
		syntaxError = true;
            }

            break;
	case UNKNOWN:
	    syntaxError = true;
	default:
	case DELIMITER:
	    break;
	}

	if (val != DELIMITER && options->Verbosity() > 3)
	{
	    stringstream msg;
	    msg << "value is " << keyWordString[val - '?'];
	    if (returnString != NULL)
		msg << ", returnString is " << returnString;
	    msg << endl;
	    xpMsg(msg.str(), __FILE__, __LINE__);
	}

	delete [] returnString;

        if (syntaxError)
        {
	    stringstream errStr;
            errStr << "Syntax error in satellite file\n";
            errStr << "line is \"" << line << "\"" << endl;
	    xpWarn(errStr.str(), __FILE__, __LINE__);
            return;
        }

	if (val == ENDOFLINE) break;
    }

    if (satellite == NULL) 
    {
	
	stringstream errStr;
	errStr << "No satellite found for  \"" << line << "\"" << endl;
	xpWarn(errStr.str(), __FILE__, __LINE__);
	return;
    }

    // Load TLE data here since select_ephemeris() in libsgp4sdp4
    // changes it.  This is in case the user wants to have two entries
    // with the same satellite.
    satellite->loadTLE();

    time_t startTime = static_cast<time_t> (options->getTVSec() + trailStart * 60);
    time_t endTime = static_cast<time_t> (options->getTVSec() + trailEnd * 60);
    time_t interval = static_cast<time_t> (trailInterval * 60);

    if (startTime > endTime)
    {
	time_t tmp = startTime;
	startTime = endTime;
	endTime = tmp;
    }

    double lat, lon, alt;
    satellite->getSpherical(startTime, lat, lon, alt);

    for (time_t t = startTime + interval; t <= endTime; t += interval)
    {
	const double prevLat = lat;
	const double prevLon = lon;
	const double prevAlt = alt;

	satellite->getSpherical(t, lat, lon, alt);

	drawArc(prevLat, prevLon, prevAlt, lat, lon, alt, color,
		spacing * deg_to_rad, planetProperties->Magnify(),
		planet, view, projection, annotationMap);
    }

    satellite->getSpherical(options->getTVSec(), lat, lon, alt);

    double X, Y, Z;
    if (sphericalToPixel(lat, lon, alt * planetProperties->Magnify(), 
			 X, Y, Z, planet, view, projection))
    {
	const int ix = static_cast<int> (floor(X + 0.5));
	const int iy = static_cast<int> (floor(Y + 0.5));
	    
	int xOffset = 0;
	int yOffset = 0;
	if (image.empty())
	{
	    Symbol *sym = new Symbol(color, ix, iy, symbolSize);
	    annotationMap.insert(pair<const double, Annotation*>(Z, sym));
	    xOffset = symbolSize;
	    yOffset = symbolSize;
	}
	else if (image.compare("none") != 0)
	{
	    unsigned char *transparent = (transparency ? transparent_pixel : NULL);
	    Icon *icon = new Icon(ix, iy, image, transparent);
	    annotationMap.insert(pair<const double, Annotation*>(Z, icon));
	    xOffset = icon->Width() / 2;
	    yOffset = icon->Height() / 2;
	}
	    
	if (!name.empty())
	{
	    Text *t = new Text(color, ix, iy, xOffset, yOffset, align, name);
	    if (!font.empty()) t->Font(font);
	    if (fontSize > 0) t->FontSize(fontSize);
		
	    annotationMap.insert(pair<const double, Annotation*>(Z, t));
	}
    }

    vector<double>::iterator a = altcirc.begin();
    while (a != altcirc.end())
    {
	// Given the angle of the spacecraft above the horizon,
	// compute the great arc distance from the sub-spacecraft
	// point
	const double r = *a - asin(sin(*a)/alt);
	drawCircle(lat, lon, r, color, spacing * deg_to_rad, 
		   planetProperties->Magnify(), planet, view,
		   projection, annotationMap);
	a++;
    }

}

void
loadSatelliteVector(PlanetProperties *planetProperties)
{
    vector<string> satfiles = planetProperties->SatelliteFiles();
    vector<string>::iterator ii = satfiles.begin();

    while (ii != satfiles.end()) 
    {
	string tleFile = *ii + ".tle";
	const bool foundFile = findFile(tleFile, "satellites");
	if (foundFile)
	{
	    ifstream inFile(tleFile.c_str());
	    char lines[3][80];
	    while (inFile.getline(lines[0], 80) != NULL)
	    {
		if ((inFile.getline(lines[1], 80) == NULL) 
		    || (inFile.getline(lines[2], 80) == NULL))
		{
		    stringstream errStr;
		    errStr << "Malformed TLE file (" << tleFile << ")?\n";
		    xpWarn(errStr.str(), __FILE__, __LINE__);
		    break;
		}
		
		Satellite sat(lines);
		
		if (!sat.isGoodData()) 
		{
		    stringstream errStr;
		    errStr << "Bad TLE data in " << tleFile << endl;
		    xpWarn(errStr.str(), __FILE__, __LINE__);
		    continue;
		}
		
		satelliteVector.push_back(sat);
	    }
	    
	    inFile.close();
	}
	else
	{
	    stringstream errStr;
	    errStr << "Can't load satellite TLE file " << tleFile << endl;
	    xpWarn(errStr.str(), __FILE__, __LINE__);
	}
	ii++;
    }
}

void
addSatellites(PlanetProperties *planetProperties, Planet *planet, 
	      View *view, ProjectionBase *projection, 
	      multimap<double, Annotation *> &annotationMap)
{
    if (planet->Index() != EARTH) return;

    vector<string> satfiles = planetProperties->SatelliteFiles();
    vector<string>::iterator ii = satfiles.begin();

    while (ii != satfiles.end()) 
    {
	string satFile(*ii);
	bool foundFile = findFile(satFile, "satellites");
	if (foundFile)
	{
	    ifstream inFile(satFile.c_str());
	    char *line = new char[256];
	    while (inFile.getline (line, 256, '\n') != NULL)
		readSatelliteFile(line, planet, view, projection,
				  planetProperties, annotationMap);
	    
	    inFile.close();
	    delete [] line;
	}
	else
	{
	    stringstream errStr;
	    errStr << "Can't load satellite file " << satFile << endl;
	    xpWarn(errStr.str(), __FILE__, __LINE__);
	}
	ii++;
    }
}

