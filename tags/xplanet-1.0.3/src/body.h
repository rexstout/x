#ifndef BODY_H
#define BODY_H

enum body
{
    SUN, 
    MERCURY, 
    VENUS, 
    EARTH, MOON, 
    MARS, PHOBOS, DEIMOS, 
    JUPITER, IO, EUROPA, GANYMEDE, CALLISTO,
    SATURN, MIMAS, ENCELADUS, TETHYS, DIONE, RHEA, TITAN, HYPERION, IAPETUS, PHOEBE, 
    URANUS, MIRANDA, ARIEL, UMBRIEL, TITANIA, OBERON, 
    NEPTUNE, TRITON, NEREID, 
    PLUTO, CHARON, 
    RANDOM_BODY,    // RANDOM_BODY needs to be after the last "real" body
    ABOVE_ORBIT, BELOW_ORBIT, DEFAULT, MAJOR_PLANET, SAME_SYSTEM, UNKNOWN_BODY
};

const char* const body_string[RANDOM_BODY] =
{"sun", 
 "mercury", 
 "venus", 
 "earth", "moon", 
 "mars", "phobos", "deimos", 
 "jupiter", "io", "europa", "ganymede", "callisto", 
 "saturn", "mimas", "enceladus", "tethys", "dione", "rhea", "titan", "hyperion", "iapetus", "phoebe", 
 "uranus", "miranda", "ariel", "umbriel", "titania", "oberon", 
 "neptune", "triton", "nereid", 
 "pluto", "charon"};

#endif
