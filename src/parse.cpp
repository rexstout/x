#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>
using namespace std;

#include "keywords.h"
#include "xpUtil.h"

extern void 
parseColor(string color, unsigned char RGB[3]);

bool
isDelimiter(char c)
{
    return(c == ' ' || c == '\t' || c == ',');
}

bool
isEndOfLine(char c)
{
    // 13 is DOS end-of-line, 28 is the file separator
    return(c == '#' || c == '\0' || c == 13 || c == 28); 
}

static void
skipPastToken(int &i, const char *line, const char endChar)
{
    while (line[i] != endChar)
    {
        if (isEndOfLine(line[i])) 
        {
	    stringstream errStr;
	    errStr << "Malformed line:\n\t" << line << "\n";
	    xpWarn(errStr.str(), __FILE__, __LINE__);

            return;
        }
        i++;
    }
}

static void
skipPastToken(int &i, const char *line)
{
    while (!isDelimiter(line[i]))
    {
        if (isEndOfLine(line[i])) return;
        i++;
    }
}

static bool
getValue(const char *line, int &i, const char *key, const char endChar, 
         char *&returnstring)
{
    const unsigned int length = strlen(key);
    if (strncmp(line + i, key, length) == 0)
    {
        i += length;
        int istart = i;
        skipPastToken(i, line, endChar);
        returnstring = new char[i - istart + 1];
        strncpy(returnstring, (line + istart), i - istart);
        returnstring[i-istart] = '\0';
        i++;
        return(true);
    }
    return(false);
}

static bool
getValue(const char *line, int &i, const char *key, char *&returnstring)
{
    const unsigned int length = strlen(key);
    if (strncmp(line + i, key, length) == 0)
    {
        i += length;
        int istart = i;
        skipPastToken(i, line);
        returnstring = new char[i - istart + 1];
        strncpy(returnstring, (line + istart), i - istart);
        returnstring[i-istart] = '\0';
        i++;
        return(true);
    }
    return(false);
}

// This routine returns the next token in the line and its type.
int 
parse(int &i, const char *line, char *&returnString)
{
    if (i >= (int) strlen(line)) return(ENDOFLINE);

    if (returnString != NULL) 
	xpWarn("returnString is not NULL!\n", __FILE__, __LINE__);

    int returnval = UNKNOWN;

    if (isDelimiter(line[i]))
    {
        i++;
        returnval = DELIMITER;
    }
    else if (isEndOfLine(line[i]))
        returnval = ENDOFLINE;
    else if (getValue(line, i, "align=", returnString))
        returnval = ALIGN;
    else if (getValue(line, i, "arc_color={", '}', returnString))
        returnval = ARC_COLOR;
    else if (getValue(line, i, "arc_color=", returnString))
    {
        unsigned char RGB[3];
        parseColor(returnString, RGB);
        delete [] returnString;
        returnString = new char[32];
        snprintf(returnString, 32, "%d,%d,%d", RGB[0], RGB[1], RGB[2]);
        returnval = ARC_COLOR;
    }
    else if (getValue(line, i, "arc_file=", returnString))
        returnval = ARC_FILE;
    else if (getValue(line, i, "[", ']', returnString))
        returnval = BODY;
    else if (getValue(line, i, "altcirc=", returnString))
        returnval = CIRCLE;
    else if (getValue(line, i, "circle=", returnString))
        returnval = CIRCLE;
    else if (getValue(line, i, "cloud_gamma=", returnString))
        returnval = CLOUD_GAMMA;
    else if (getValue(line, i, "cloud_map=", returnString))
        returnval = CLOUD_MAP;
    else if (getValue(line, i, "cloud_threshold=", returnString))
        returnval = CLOUD_THRESHOLD;
    else if (getValue(line, i, "color={", '}', returnString))
        returnval = COLOR;
    else if (getValue(line, i, "color=", returnString))
    {
        unsigned char RGB[3];
        parseColor(returnString, RGB);
        delete [] returnString;
        returnString = new char[32];
        snprintf(returnString, 32, "%d,%d,%d", RGB[0], RGB[1], RGB[2]);
        returnval = COLOR;
    }
    else if (getValue(line, i, "draw_orbit=", returnString))
        returnval = DRAW_ORBIT;
    else if (getValue(line, i, "font=", returnString))
        returnval = FONT;
    else if (getValue(line, i, "fontsize=", returnString))
        returnval = FONTSIZE;
    else if (getValue(line, i, "grid=", returnString))
        returnval = GRID;
    else if (getValue(line, i, "grid1=", returnString))
        returnval = GRID1;
    else if (getValue(line, i, "grid2=", returnString))
        returnval = GRID2;
    else if (getValue(line, i, "grid_color=", returnString))
        returnval = GRID_COLOR;
    else if (getValue(line, i, "image=", returnString))
        returnval = IMAGE;
    else if (getValue(line, i, "\"", '"', returnString))
        returnval = NAME;
    else if (getValue(line, i, "{", '}', returnString))
        returnval = NAME;
    else if (getValue(line, i, "magnify=", returnString))
        returnval = MAGNIFY;
    else if (getValue(line, i, "mapbounds={", '}', returnString))
	returnval = MAP_BOUNDS;
    else if (getValue(line, i, "marker_color={", '}', returnString))
        returnval = MARKER_COLOR;
    else if (getValue(line, i, "marker_color=", returnString))
    {
        unsigned char RGB[3];
        parseColor(returnString, RGB);
        delete [] returnString;
        returnString = new char[32];
        snprintf(returnString, 32, "%d,%d,%d", RGB[0], RGB[1], RGB[2]);
        returnval = MARKER_COLOR;
    }
    else if (getValue(line, i, "marker_file=", returnString))
        returnval = MARKER_FILE;
    else if (getValue(line, i, "marker_font=", returnString))
        returnval = MARKER_FONT;
    else if (getValue(line, i, "map=", returnString))
        returnval = DAY_MAP;
    else if (getValue(line, i, "max_radius_for_label=", returnString))
        returnval = MAX_RAD_FOR_LABEL;
    else if (getValue(line, i, "min_radius_for_label=", returnString))
        returnval = MIN_RAD_FOR_LABEL;
    else if (getValue(line, i, "min_radius_for_markers=", returnString))
        returnval = MIN_RAD_FOR_MARKERS;
    else if (getValue(line, i, "night_map=", returnString))
        returnval = NIGHT_MAP;
    else if (getValue(line, i, "orbit={", '}', returnString))
        returnval = ORBIT;
    else if (getValue(line, i, "orbit_color={", '}', returnString))
        returnval = ORBIT_COLOR;
    else if (getValue(line, i, "position=", returnString))
        returnval = POSITION;
    else if (getValue(line, i, "radius=", returnString))
        returnval = RADIUS;
    else if (getValue(line, i, "random_origin=", returnString))
        returnval = RANDOM_ORIGIN;
    else if (getValue(line, i, "random_target=", returnString))
        returnval = RANDOM_TARGET;
    else if (getValue(line, i, "satellite_file=", returnString))
        returnval = SATELLITE_FILE;
    else if (getValue(line, i, "shade=", returnString))
        returnval = SHADE;
    else if (getValue(line, i, "spacing=", returnString))
        returnval = SPACING;
    else if (getValue(line, i, "specular_map=", returnString))
        returnval = SPECULAR_MAP;
    else if (getValue(line, i, "symbolsize=", returnString))
        returnval = SYMBOLSIZE;
    else if (getValue(line, i, "text_color={", '}', returnString))
        returnval = TEXT_COLOR;
    else if (strncmp(line+i, "timezone=", 9) == 0)
    {
        i += 9;
        int istart = i;
        while (line[i] == '/' || line[i] == ',' || !isDelimiter(line[i]))
        {
            if (isEndOfLine(line[i])) break;
            i++;
        }
        returnString = new char[i - istart + 1];
        strncpy(returnString, (line + istart), i - istart);
        returnString[i-istart] = '\0';
        returnval = TIMEZONE;
    }
    else if (getValue(line, i, "trail={", '}', returnString))
        returnval = TRAIL;
    else if (getValue(line, i, "transparent={", '}', returnString))
        returnval = TRANSPARENT;
    else // assume it's a latitude/longitude value
    {
        int istart = i;
        skipPastToken(i, line);
        returnString = new char[i - istart + 1];
        strncpy(returnString, (line + istart), i - istart);
        returnString[i-istart] = '\0';

        returnval = LATLON;
    }

    return(returnval);
}
