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
    return(c == ' ' || c == '\t');
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
            ostringstream errStr;
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

    int returnVal = UNKNOWN;

    if (isDelimiter(line[i]))
    {
        i++;
        returnVal = DELIMITER;
    }
    else if (isEndOfLine(line[i]))
        returnVal = ENDOFLINE;
    else if (getValue(line, i, "align=", returnString))
        returnVal = ALIGN;
    else if (getValue(line, i, "arc_color={", '}', returnString))
        returnVal = ARC_COLOR;
    else if (getValue(line, i, "arc_color=", returnString))
    {
        unsigned char RGB[3];
        parseColor(returnString, RGB);
        delete [] returnString;
        returnString = new char[32];
        snprintf(returnString, 32, "%d,%d,%d", RGB[0], RGB[1], RGB[2]);
        returnVal = ARC_COLOR;
    }
    else if (getValue(line, i, "arc_file=", returnString))
        returnVal = ARC_FILE;
    else if (getValue(line, i, "[", ']', returnString))
        returnVal = BODY;
    else if (getValue(line, i, "altcirc=", returnString))
        returnVal = CIRCLE;
    else if (getValue(line, i, "circle=", returnString))
        returnVal = CIRCLE;
    else if (getValue(line, i, "cloud_gamma=", returnString))
        returnVal = CLOUD_GAMMA;
    else if (getValue(line, i, "cloud_map=", returnString))
        returnVal = CLOUD_MAP;
    else if (getValue(line, i, "cloud_ssec=", returnString))
        returnVal = CLOUD_SSEC;
    else if (getValue(line, i, "cloud_threshold=", returnString))
        returnVal = CLOUD_THRESHOLD;
    else if (getValue(line, i, "color={", '}', returnString))
        returnVal = COLOR;
    else if (getValue(line, i, "color=", returnString))
    {
        unsigned char RGB[3];
        parseColor(returnString, RGB);
        delete [] returnString;
        returnString = new char[32];
        snprintf(returnString, 32, "%d,%d,%d", RGB[0], RGB[1], RGB[2]);
        returnVal = COLOR;
    }
    else if (getValue(line, i, "draw_orbit=", returnString))
        returnVal = DRAW_ORBIT;
    else if (getValue(line, i, "font=", returnString))
        returnVal = FONT;
    else if (getValue(line, i, "fontsize=", returnString))
        returnVal = FONTSIZE;
    else if (getValue(line, i, "grid=", returnString))
        returnVal = GRID;
    else if (getValue(line, i, "grid1=", returnString))
        returnVal = GRID1;
    else if (getValue(line, i, "grid2=", returnString))
        returnVal = GRID2;
    else if (getValue(line, i, "grid_color=", returnString))
        returnVal = GRID_COLOR;
    else if (getValue(line, i, "image=", returnString))
        returnVal = IMAGE;
    else if (getValue(line, i, "lang=", returnString))
        returnVal = LANGUAGE;
    else if (getValue(line, i, "magnify=", returnString))
        returnVal = MAGNIFY;
    else if (getValue(line, i, "mapbounds={", '}', returnString))
        returnVal = MAP_BOUNDS;
    else if (getValue(line, i, "marker_color={", '}', returnString))
        returnVal = MARKER_COLOR;
    else if (getValue(line, i, "marker_color=", returnString))
    {
        unsigned char RGB[3];
        parseColor(returnString, RGB);
        delete [] returnString;
        returnString = new char[32];
        snprintf(returnString, 32, "%d,%d,%d", RGB[0], RGB[1], RGB[2]);
        returnVal = MARKER_COLOR;
    }
    else if (getValue(line, i, "marker_file=", returnString))
        returnVal = MARKER_FILE;
    else if (getValue(line, i, "marker_font=", returnString))
        returnVal = MARKER_FONT;
    else if (getValue(line, i, "map=", returnString))
        returnVal = DAY_MAP;
    else if (getValue(line, i, "max_radius_for_label=", returnString))
        returnVal = MAX_RAD_FOR_LABEL;
    else if (getValue(line, i, "min_radius_for_label=", returnString))
        returnVal = MIN_RAD_FOR_LABEL;
    else if (getValue(line, i, "min_radius_for_markers=", returnString))
        returnVal = MIN_RAD_FOR_MARKERS;
    else if (getValue(line, i, "\"", '"', returnString))
        returnVal = NAME;
    else if (getValue(line, i, "{", '}', returnString))
        returnVal = NAME;
    else if (getValue(line, i, "night_map=", returnString))
        returnVal = NIGHT_MAP;
    else if (getValue(line, i, "orbit={", '}', returnString))
        returnVal = ORBIT;
    else if (getValue(line, i, "orbit_color={", '}', returnString))
        returnVal = ORBIT_COLOR;
    else if (getValue(line, i, "relative_to=", returnString))
        returnVal = ORIGIN;
    else if (getValue(line, i, "position=", returnString))
        returnVal = POSITION;
    else if (getValue(line, i, "radius=", returnString))
        returnVal = RADIUS;
    else if (getValue(line, i, "random_origin=", returnString))
        returnVal = RANDOM_ORIGIN;
    else if (getValue(line, i, "random_target=", returnString))
        returnVal = RANDOM_TARGET;
    else if (getValue(line, i, "satellite_file=", returnString))
        returnVal = SATELLITE_FILE;
    else if (getValue(line, i, "shade=", returnString))
        returnVal = SHADE;
    else if (getValue(line, i, "spacing=", returnString))
        returnVal = SPACING;
    else if (getValue(line, i, "specular_map=", returnString))
        returnVal = SPECULAR_MAP;
    else if (getValue(line, i, "symbolsize=", returnString))
        returnVal = SYMBOLSIZE;
    else if (getValue(line, i, "text_color={", '}', returnString))
        returnVal = TEXT_COLOR;
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
        returnVal = TIMEZONE;
    }
    else if (getValue(line, i, "trail={", '}', returnString))
        returnVal = TRAIL;
    else if (getValue(line, i, "transparent={", '}', returnString))
        returnVal = TRANSPARENT;
    else if (getValue(line, i, "twilight=", returnString))
        returnVal = TWILIGHT;
    else // assume it's a latitude/longitude value
    {
        int istart = i;
        skipPastToken(i, line);
        returnString = new char[i - istart + 1];
        strncpy(returnString, (line + istart), i - istart);
        returnString[i-istart] = '\0';

        returnVal = LATLON;
    }

    return(returnVal);
}
