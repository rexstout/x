#include <clocale>
#include <cstdio>
#include <map>
#include <sstream>
#include <vector>
using namespace std;

#include <sys/time.h>
#include <unistd.h>

#include "config.h"

#include "buildPlanetMap.h"
#include "keywords.h"
#include "Options.h"
#include "PlanetProperties.h"
#include "readOriginFile.h"
#include "xpUtil.h"

#include "libannotate/libannotate.h"
#include "libdisplay/libdisplay.h"
#include "libdisplay/libtimer.h"
#include "libplanet/Planet.h"

extern void
drawMultipleBodies(DisplayBase *display, Planet *target,
                   map<double, Planet *> &planetsFromSunMap,
                   PlanetProperties *planetProperties[]);

extern void
drawProjection(DisplayBase *display, Planet *target,
               map<double, Planet *> &planetsFromSunMap,
               PlanetProperties *planetProperties);

extern void
readConfigFile(string configFile, PlanetProperties *planetProperties[]);

void
xplanet_main(int argc, char **argv)
{
    setlocale(LC_ALL, "");

    Options *options = Options::getInstance();
    options->parseArgs(argc, argv);

    PlanetProperties *planetProperties[RANDOM_BODY];
    for (int i = 0; i < RANDOM_BODY; i++)
        planetProperties[i] = new PlanetProperties((body) i);

    // Load up the drawing info for each planet
    readConfigFile(options->ConfigFile(), planetProperties);

    // Load up artificial satellite orbital elements
    if (!planetProperties[EARTH]->SatelliteFiles().empty())
        loadSatelliteVector(planetProperties[EARTH]);

    // If an origin file has been specified, read it
    const bool origin_file = !options->OriginFile().empty();
    vector<LBRPoint> originVector;
    if (origin_file) 
    {
        readOriginFile(options->OriginFile(), originVector);
        options->NumTimes(originVector.size());
    }

    vector<LBRPoint>::iterator iterOriginVector = originVector.begin();

    // Initialize the timer
    Timer *timer = getTimer(options->getWait(), options->Hibernate(),
			    options->IdleWait());

    int times_run = 0;

    while (1)
    {
        // Set the time for the next update
        timer->Update();

        if (!options->PrevCommand().empty())
        {
            if (system(options->PrevCommand().c_str()) != 0)
	    {
		stringstream errStr;
		errStr << "Can't execute " << options->PrevCommand() << "\n";
		xpWarn(errStr.str(), __FILE__, __LINE__);
	    }
        }

        // Initialize display device
        DisplayBase *display = getDisplay(times_run);

        // Set the time to the current time, if desired
        if (options->UseCurrentTime())
        {
            struct timeval time;
            gettimeofday(&time, NULL);
            
            const double julianDay = toJulian(gmtime((time_t *) &time.tv_sec)->tm_year + 1900,
                                              gmtime((time_t *) &time.tv_sec)->tm_mon + 1,
                                              gmtime((time_t *) &time.tv_sec)->tm_mday,
                                              gmtime((time_t *) &time.tv_sec)->tm_hour,
                                              gmtime((time_t *) &time.tv_sec)->tm_min,
                                              gmtime((time_t *) &time.tv_sec)->tm_sec);
            options->setTime(julianDay);
        }
        
        if (origin_file)
        {
            // Set the time & position
            options->setTime(iterOriginVector->time);
            options->Latitude(iterOriginVector->latitude);
            options->Longitude(iterOriginVector->longitude);
            options->setRange(iterOriginVector->radius);
        }
    
        options->setTarget(planetProperties);
        options->setOrigin(planetProperties);

        // Rectangular coordinates of the observer
        double oX, oY, oZ;
        options->getOrigin(oX, oY, oZ);

        // Calculate the positions of the planets & moons.  The map
        // container sorts on the key, so the bodies will be ordered
        // by heliocentric distance.  This makes calculating shadows
        // easier.
        map<double, Planet *> planetsFromSunMap;
        buildPlanetMap(options->getJulianDay(), oX, oY, oZ, 
		       options->LightTime(), 
                       planetsFromSunMap);

        // Find the target body in the list
        body target_body = options->getTarget();
        Planet *target = findPlanetinMap(planetsFromSunMap, 
                                         target_body);
        if (target == NULL)
            xpExit("Can't find target body?", __FILE__, __LINE__);

        if (options->LightTime())
        {
            double tX, tY, tZ;
            target->getPosition(tX, tY, tZ);
            options->setTarget(tX, tY, tZ);
        }

        // Find the sub-observer lat & lon
        double obs_lat, obs_lon;
        target->XYZToPlanetocentric(oX, oY, oZ, obs_lat, obs_lon);
        options->Latitude(obs_lat);
        options->Longitude(obs_lon);

	// delete the markerbounds file, since we'll create a new one
	string markerBounds(options->MarkerBounds());
	if (!markerBounds.empty())
	    unlinkFile(markerBounds.c_str());

        if (options->Projection() == MULTIPLE)
            drawMultipleBodies(display, target, planetsFromSunMap,
                               planetProperties);
        else
            drawProjection(display, target, planetsFromSunMap, 
                           planetProperties[target->Index()]);

        display->renderImage(planetProperties);
        delete display;

        destroyPlanetMap();

        times_run++;

       if (!options->PostCommand().empty())
        {
            if (system(options->PostCommand().c_str()) != 0)
	    {
		stringstream errStr;
		errStr << "Can't execute " << options->PostCommand() << "\n";
		xpWarn(errStr.str(), __FILE__, __LINE__);
	    }
        }

        if (options->NumTimes() > 0
            && options->NumTimes() <= times_run) 
            break;

        if (origin_file)
        {
            // If we've run through the origin file, break out of the
            // while(1) loop.
            iterOriginVector++;
            if (iterOriginVector == originVector.end()) break;
        }
        else if (!options->UseCurrentTime())
        {
            // Set the time to the next update
            options->incrementTime((time_t) (options->getTimeWarp() 
                                             * options->getWait()));
        }

        // Sleep until the next update.  If Sleep() returns false,
        // then quit.
        if (!timer->Sleep()) break;
    }

    delete timer;

    for (int i = 0; i < RANDOM_BODY; i++) delete planetProperties[i];
}

//#ifndef HAVE_AQUA
int
main(int argc, char **argv)
{
    xplanet_main(argc, argv);
    return(EXIT_SUCCESS);
}
//#endif
