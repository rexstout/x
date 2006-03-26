#ifndef KEYWORDS_H
#define KEYWORDS_H

enum keyWords
{
    UNKNOWN = '?',            // for getopt
    ABOVE, ABSOLUTE, ALIGN, ANCIENT, ARC_COLOR, ARC_FILE, AUTO, AZIMUTHAL, 
    BACKGROUND, BASEMAG, BELOW, BODY, 
    CENTER, CIRCLE, CLOUD_GAMMA, CLOUD_MAP, CLOUD_THRESHOLD, COLOR, CONFIG_FILE, 
    DATE, DATE_FORMAT, DAY_MAP, DELIMITER, DRAW_ORBIT, DYNAMIC_ORIGIN,
    ENDOFLINE, EPHEMERIS, 
    FONT, FONTSIZE, FORK, FOV, 
    GALACTIC, GEOMETRY, GRID, GRID1, GRID2, GRID_COLOR, GROUND, 
    HEMISPHERE, HIBERNATE,
    IDLEWAIT, IMAGE, INTERPOLATE_ORIGIN_FILE,
    JDATE, JPL_FILE, 
    LABEL, LABELPOS, LABEL_STRING, LAMBERT, LANGUAGE, LATITUDE, LATLON, LBR, LEFT, LIGHT_TIME, LOCALTIME, LOGMAGSTEP, LONGITUDE, LOOKAT, 
    MAGNIFY, MAJOR, MAKECLOUDMAPS, MAP_BOUNDS, MARKER_BOUNDS, MARKER_COLOR, MARKER_FILE, MARKER_FONT, MAX_RAD_FOR_LABEL, MIN_RAD_FOR_LABEL, MIN_RAD_FOR_MARKERS, MERCATOR, MOLLWEIDE, MULTIPLE,
    NAME, NIGHT_MAP, NORTH, NUM_TIMES, 
    OUTPUT, ORBIT, ORBIT_COLOR, ORIGIN, ORIGINFILE, ORTHOGRAPHIC, 
    PANGO, PETERS, POSITION, POST_COMMAND, PREV_COMMAND, PROJECTION, 
    QUALITY, 
    RADIUS, RANDOM, RANDOM_ORIGIN, RANDOM_TARGET, RANGE, RECTANGULAR, RIGHT, ROOT, ROTATE, 
    SATELLITE_FILE, SAVE_DESKTOP_FILE, SEARCHDIR, SHADE, SPACING, SPECULAR_MAP, SPICE_EPHEMERIS, SPICE_FILE, STARFREQ, STARMAP, SYMBOLSIZE, SYSTEM, 
    TARGET, TERRESTRIAL, TEXT_COLOR, TIMEWARP, TIMEZONE, TMPDIR, TRAIL, TRANSPARENT, TRANSPNG, TWILIGHT,
    UTCLABEL, 
    VERBOSITY, VERSIONNUMBER, VROOT, 
    WAIT, WINDOW, WINDOWTITLE, 
    XWINID,
    LAST_WORD
};

const char * const keyWordString[LAST_WORD] =
{
    "UNKNOWN",
    "ABOVE", "ABSOLUTE", "ALIGN", "ANCIENT", "ARC_COLOR", "ARC_FILE", "AUTO", "AZIMUTHAL", 
    "BACKGROUND", "BASEMAG", "BELOW", "BODY", 
    "CENTER", "CIRCLE", "CLOUD_GAMMA", "CLOUD_MAP", "CLOUD_THRESHOLD", "COLOR", "CONFIG_FILE", 
    "DATE", "DATE_FORMAT", "DAY_MAP", "DELIMITER", "DRAW_ORBIT", "DYNAMIC_ORIGIN",
    "ENDOFLINE", "EPHEMERIS", 
    "FONT", "FONTSIZE", "FORK", "FOV", 
    "GALACTIC", "GEOMETRY", "GRID", "GRID1", "GRID2", "GRID_COLOR", "GROUND", 
    "HEMISPHERE", "HIBERNATE",
    "IDLEWAIT", "IMAGE", "INTERPOLATE_ORIGIN_FILE",
    "JDATE", "JPL_FILE", 
    "LABEL", "LABELPOS", "LABEL_STRING", "LAMBERT", "LANGUAGE", "LATITUDE", "LATLON", "LBR", "LEFT", "LIGHT_TIME", "LOCALTIME", "LOGMAGSTEP", "LONGITUDE", "LOOKAT", 
    "MAGNIFY", "MAJOR", "MAKECLOUDMAPS", "MAP_BOUNDS", "MARKER_BOUNDS", "MARKER_COLOR", "MARKER_FILE", "MARKER_FONT", "MAX_RAD_FOR_LABEL", "MIN_RAD_FOR_LABEL", "MIN_RAD_FOR_MARKERS", "MERCATOR", "MOLLWEIDE", "MULTIPLE",
    "NAME", "NIGHT_MAP", "NORTH", "NUM_TIMES", 
    "OUTPUT", "ORBIT", "ORBIT_COLOR", "ORIGIN", "ORIGINFILE", "ORTHOGRAPHIC", 
    "PANGO", "PETERS", "POSITION", "POST_COMMAND", "PREV_COMMAND", "PROJECTION", 
    "QUALITY", 
    "RADIUS", "RANDOM", "RANDOM_ORIGIN", "RANDOM_TARGET", "RANGE", "RECTANGULAR", "RIGHT", "ROOT", "ROTATE", 
    "SATELLITE_FILE", "SAVE_DESKTOP_FILE", "SEARCHDIR", "SHADE", "SPACING", "SPECULAR_MAP", "SPICE_EPHEMERIS", "SPICE_FILE", "STARFREQ", "STARMAP", "SYMBOLSIZE", "SYSTEM", 
    "TARGET", "TERRESTRIAL", "TEXT_COLOR", "TIMEWARP", "TIMEZONE", "TMPDIR", "TRAIL", "TRANSPARENT", "TRANSPNG", "TWILIGHT",
    "UTCLABEL", 
    "VERBOSITY", "VERSIONNUMBER", "VROOT", 
    "WAIT", "WINDOW", "WINDOWTITLE", 
    "XWINID", 
    "LAST_WORD"
};

#endif
