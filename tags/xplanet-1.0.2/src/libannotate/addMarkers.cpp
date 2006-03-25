#include <iostream>
#include <cstdio>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

#include "buildPlanetMap.h"
#include "findFile.h"
#include "keywords.h"
#include "Options.h"
#include "parse.h"
#include "PlanetProperties.h"
#include "sphericalToPixel.h"
#include "View.h"
#include "xpUtil.h"

#include "libannotate/libannotate.h"
#include "libplanet/Planet.h"
#include "libprojection/ProjectionBase.h"

static void
readMarkerFile(const char *line, Planet *planet, 
               const double pX, const double pY, const double pZ,
               View *view, ProjectionBase *projection,
               const int width, const int height, 
               unsigned char *color, string &font, const double magnify,
               map<double, Planet *> &planetsFromSunMap, 
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

    int align = AUTO;
    int fontSize = -1;
    bool haveLat = false;
    double lat, lon;
    string image;
    string name("");
    bool pixelCoords = false;
    double radius = 1;
    bool relativeToEdges = true;
    int symbolSize = 2;
    bool syntaxError = false;
    string timezone;
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
                xpWarn("Unrecognized option for align in marker file\n",
                       __FILE__, __LINE__);
                syntaxError = true;
                break;
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
                xpWarn("Need three values for color\n", 
                       __FILE__, __LINE__);
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
                xpWarn("fontSize must be positive.\n", 
                       __FILE__, __LINE__);
                syntaxError = true;
            }
            break;
        case IMAGE:
            image.assign(returnString);
            break;
        case LATLON:
            if (haveLat)
            {
                sscanf(returnString, "%lf", &lon);
            }
            else
            {
                sscanf(returnString, "%lf", &lat);
                haveLat = true;
            }
            break;
        case NAME:
            name.assign(returnString);
            break;
        case POSITION:
            if (strncmp(returnString, "pixel", 2) == 0)
            {
                pixelCoords = true;
            }
            else if (strncmp(returnString, "absolute", 3) == 0)
            {
                pixelCoords = true;
                relativeToEdges = false;
            }
            else
            {
                if (planet != NULL)
                {
                    body pIndex = Planet::parseBodyName(returnString);
                    
                    if (pIndex != planet->Index())
                    {
                        const Planet *other = findPlanetinMap(planetsFromSunMap, 
                                                              pIndex);
                        double X, Y, Z;
                        other->getPosition(X, Y, Z);
                        planet->XYZToPlanetocentric(X, Y, Z, lat, lon);
                        
                        lat /= deg_to_rad;
                        lon /= deg_to_rad;
                    }
                }
            }
        break;
        case RADIUS:
            sscanf(returnString, "%lf", &radius);
            if (radius < 0) 
            {
                xpWarn("Radius value must be positive\n",
                       __FILE__, __LINE__);
                radius = 1;
                syntaxError = true;
            }
            break;
        case SYMBOLSIZE:
            sscanf(returnString, "%d", &symbolSize);
            if (symbolSize < 0) symbolSize = 2;
            break;
        case TIMEZONE:
            timezone.assign(returnString);
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
                xpWarn("Need three values for transparency pixel!\n",
                       __FILE__, __LINE__);
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
            errStr << "Syntax error in marker file\n"
                   << "line is \"" << line << "\"\n";
            xpWarn(errStr.str(), __FILE__, __LINE__);
            return;
        }

        if (val == ENDOFLINE) break;
    }

    double X, Y, Z = 0;
    bool markerVisible = false;

    if (pixelCoords)
    {   
        X = lon;
        Y = lat;

        if (relativeToEdges)
        {
            if (X < 0) X += width;
            if (Y < 0) Y += height;
        }
    }
    else
    {
        lat *= deg_to_rad;
        lon *= deg_to_rad;

        markerVisible = sphericalToPixel(lat, lon, radius * magnify, 
                                         X, Y, Z, planet, view, projection);

        // don't draw markers on the far side of the planet
        if (planet != NULL && view != NULL && Z > pZ) 
            markerVisible = false;
    }
        

    if (pixelCoords || markerVisible)
    {
        const int ix = static_cast<int> (floor(X + 0.5));
        const int iy = static_cast<int> (floor(Y + 0.5));

        int iconWidth = 0;
        int iconHeight = 0;
        if (image.empty())
        {
            Symbol *s = new Symbol(color, ix, iy, symbolSize);
            annotationMap.insert(pair<const double, Annotation*>(Z, s));
            iconWidth = symbolSize * 2;
            iconHeight = symbolSize * 2;
        }
        else if (image.compare("none") != 0)
        {
            unsigned char *transparent = (transparency ? transparent_pixel : NULL);
            Icon *i = new Icon(ix, iy, image, transparent);
            annotationMap.insert(pair<const double, Annotation*>(Z, i));
            iconWidth = i->Width();
            iconHeight = i->Height();
        }

        if (!timezone.empty())
        {
            char *get_tz = getenv("TZ");
            string tz_save;
            if (get_tz != NULL)
            {
                tz_save = "TZ=";
                tz_save += get_tz;
            }

            string tz = "TZ=";
            tz += timezone;
            putenv((char *) tz.c_str());

            tzset();

            char name_str[256];
            time_t tv_sec = options->getTVSec();
            strftime(name_str, sizeof(name_str), name.c_str(), 
                     localtime((time_t *) &tv_sec));
            name.assign(name_str);

            if (tz_save.empty()) 
                removeFromEnvironment("TZ"); 
            else
                putenv((char *) tz_save.c_str()); 

            tzset();
        }

        if (!name.empty())
        {
            Text *t = new Text(color, ix, iy, iconWidth, iconHeight, 
                               align, name);

            if (!font.empty()) t->Font(font);
            if (fontSize > 0) t->FontSize(fontSize);
            
            annotationMap.insert(pair<const double, Annotation*>(Z, t));
        }
    }
}

void
addMarkers(PlanetProperties *planetProperties, Planet *planet, 
           const double X, const double Y, const double Z,
           View *view, ProjectionBase *projection, 
           const int width, const int height, 
           map<double, Planet *> &planetsFromSunMap, 
           multimap<double, Annotation *> &annotationMap)
{
    vector<string> markerfiles = planetProperties->MarkerFiles();
    vector<string>::iterator ii = markerfiles.begin();

    unsigned char color[3];
    memcpy(color, planetProperties->MarkerColor(), 3);

    string font(planetProperties->MarkerFont());

    while (ii != markerfiles.end()) 
    {
        string markerFile(*ii);
        bool foundFile = findFile(markerFile, "markers");
        if (foundFile)
        {
            ifstream inFile(markerFile.c_str());
            char *line = new char[256];
            while (inFile.getline (line, 256, '\n') != NULL)
                readMarkerFile(line, planet, X, Y, Z, 
                               view, projection, width, height, 
                               color, font,
                               planetProperties->Magnify(),
                               planetsFromSunMap, annotationMap);
            
            inFile.close();
            delete [] line;
        }
        else
        {
            stringstream errStr;
            errStr << "Can't load marker file " << markerFile << endl;
            xpWarn(errStr.str(), __FILE__, __LINE__);
        }
        ii++;
    }
}

void
addMarkers(View *view, const int width, const int height, 
           map<double, Planet *> &planetsFromSunMap, 
           multimap<double, Annotation *> &annotationMap)
{
    Options *options = Options::getInstance();

    vector<string> markerfiles = options->MarkerFiles();
    vector<string>::iterator ii = markerfiles.begin();

    unsigned char color[3];
    memcpy(color, options->Color(), 3);

    string font(options->Font());

    while (ii != markerfiles.end()) 
    {
        string markerFile(*ii);
        bool foundFile = findFile(markerFile, "markers");
        if (foundFile)
        {
            ifstream inFile(markerFile.c_str());
            char *line = new char[256];
            while (inFile.getline (line, 256, '\n') != NULL)
                readMarkerFile(line, NULL, 0, 0, 0,
                               view, NULL, width, height, 
                               color, font, 1.0, 
                               planetsFromSunMap, annotationMap);
            
            inFile.close();
            delete [] line;
        }
        else
        {
            stringstream errStr;
            errStr << "Can't load marker file " << markerFile << endl;
            xpWarn(errStr.str(), __FILE__, __LINE__);
        }
        ii++;
    }
}
