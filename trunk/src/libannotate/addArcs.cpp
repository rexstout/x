#include <cstdio>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
using namespace std;

#include "findFile.h"
#include "keywords.h"
#include "Options.h"
#include "parse.h"
#include "PlanetProperties.h"
#include "sphericalToPixel.h"
#include "xpUtil.h"

#include "drawArc.h"

#include "libannotate/libannotate.h"
#include "libplanet/Planet.h"
#include "libprojection/ProjectionBase.h"

class View;

static void
readArcFile(const char *line, Planet *planet, 
	    View *view, ProjectionBase *projection,
	    PlanetProperties *planetProperties, 
	    multimap<double, Annotation *> &annotationMap)
{
    int i = 0, j = 0, k = 0;
    while (isDelimiter(line[i]))
    {
	i++;
	if (static_cast<unsigned int> (i) > strlen(line)) return;
    }
    if (isEndOfLine(line[i])) return;

    Options *options = Options::getInstance();

    unsigned char color[3];
    memcpy(color, planetProperties->ArcColor(), 3);

    double coords[4];
    double radius[2] = { 1, 1 };
    double spacing = 0.1;
    bool syntaxError = false;

    while (static_cast<unsigned int> (i) < strlen(line))
    {
	char *returnString = NULL;
	int val = parse(i, line, returnString);

	switch (val)
	{
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
		xpWarn("Need three values for color\n", 
		       __FILE__, __LINE__);
		syntaxError = true;
	    }
	}
	break;
	case LATLON:
	    if (j < 4)
	    {
                sscanf(returnString, "%lf", &coords[j]);
                if (j%2 == 0)
                {
                    if (coords[j] < -90 || coords[j] > 90)
                    {
			xpWarn("Latitude value must be between -90 and 90 degrees\n",
			       __FILE__, __LINE__);
                        syntaxError = true;
                    }
                }
                else 
                {
                    if (coords[j] < -360 || coords[j] > 360)
                    {
			xpWarn("Longitude value must be between -360 and 360 degrees\n",
			       __FILE__, __LINE__);
                        syntaxError = true;
                    }
                }
		coords[j] *= deg_to_rad;
                j++;
            }
            else
            {
                syntaxError = true;
	    }
	    break;
	case RADIUS:
	    if (k < 2)
	    {
		sscanf(returnString, "%lf", &radius[k]);
		if (radius[k] < 0) 
		{
		    xpWarn("Radius value must be positive\n",
			   __FILE__, __LINE__);
		    radius[k] = 1;
		    syntaxError = true;
		}
		else
		    k++;
	    }
	    break;
	case SPACING:
	    sscanf(returnString, "%lf", &spacing);
	    if (spacing < 0) 
	    {
		xpWarn("spacing must be positive\n",
		       __FILE__, __LINE__);
		spacing = 0.1;
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
            errStr << "Syntax error in arc file\n"
		   << "line is \"" << line << "\"\n";
	    xpWarn(errStr.str(), __FILE__, __LINE__);
            return;
        }

	if (val == ENDOFLINE) break;
    }
    
    if (j != 4)
    {
	stringstream errStr;
	errStr << "Incomplete entry in arc file\n"
	       << "line is \"" << line << "\"\n";
	xpWarn(errStr.str(), __FILE__, __LINE__);
        return;
    }

    if (k == 0) radius[1] = radius[0];

    drawArc(coords[0], coords[1], radius[0], coords[2], coords[3], radius[1], color, 
	    spacing * deg_to_rad, planetProperties->Magnify(), 
	    planet, view, projection, annotationMap);
}

void
addArcs(PlanetProperties *planetProperties, Planet *planet, 
	View *view, ProjectionBase *projection, 
	multimap<double, Annotation *> &annotationMap)
{
    vector<string> arcfiles = planetProperties->ArcFiles();
    vector<string>::iterator ii = arcfiles.begin();

    while (ii != arcfiles.end()) 
    {
	string arcFile(*ii);
	bool foundFile = findFile(arcFile, "arcs");
	if (foundFile)
	{
	    ifstream inFile(arcFile.c_str());
	    char *line = new char[256];
	    while (inFile.getline (line, 256, '\n') != NULL)
		readArcFile(line, planet, view, projection,
			    planetProperties,
			    annotationMap);
	    
	    inFile.close();
	    delete [] line;
	}
	else
	{
	    stringstream errStr;
	    errStr << "Can't load arc file " << arcFile << endl;
	    xpWarn(errStr.str(), __FILE__, __LINE__);
	}
	ii++;
    }
}

