#include <clocale>
#include <cstdio>
#include <iostream>
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
#include "libephemeris/ephemerisWrapper.h"
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

int
main(int argc, char **argv)
{
    if (setlocale(LC_ALL, "") == NULL)
    {
        ostringstream errMsg;
        errMsg << "Warning: setlocale(LC_ALL, \"\") failed! "
               << "Check your LANG environment variable "
               << "(currently ";
        char *lang = getenv("LANG");
        if (lang == NULL)
        {
            errMsg << "NULL";
        }
        else
        {
            errMsg << "\"" << lang << "\"";
        }
        errMsg << "). Setting to \"C\".\n";
        setlocale(LC_ALL, "C");
        cerr << errMsg.str();
    }

    Options *options = Options::getInstance();
    options->parseArgs(argc, argv);

    if (options->Fork())
    {
        pid_t pid = fork();
        switch (pid)
        {
        case 0:
            // This is the child process
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
            setsid();
            break;
        case -1:
            xpExit("fork() failed!\n", __FILE__, __LINE__);
            break;
        default:
            // This is the parent process
            if (options->Verbosity() > 1)
            {
                ostringstream msg;
                msg << "Forked child process, PID is " << pid << "\n";
                xpMsg(msg.str(), __FILE__, __LINE__);
            }
            return(EXIT_SUCCESS);
        }
    }

    setUpEphemeris();

    PlanetProperties *planetProperties[RANDOM_BODY];
    for (int i = 0; i < RANDOM_BODY; i++)
        planetProperties[i] = new PlanetProperties((body) i);

    // Load the drawing info for each planet
    readConfigFile(options->ConfigFile(), planetProperties);

#ifdef HAVE_CSPICE
    // Load any SPICE kernels
    processSpiceKernels(true);
#endif

    // Load artificial satellite orbital elements
    if (!planetProperties[EARTH]->SatelliteFiles().empty())
        loadSatelliteVector(planetProperties[EARTH]);

    // If an origin file has been specified, read it
    const bool origin_file = !options->OriginFile().empty();
    vector<LBRPoint> originVector;
    if (origin_file) 
    {
        readOriginFile(options->OriginFile(), originVector);
        if (!options->InterpolateOriginFile())
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
                ostringstream errStr;
                errStr << "Can't execute " << options->PrevCommand() 
                       << "\n";
                xpWarn(errStr.str(), __FILE__, __LINE__);
            }
        }

        // Set the time to the current time, if desired
        if (options->UseCurrentTime())
        {
            struct timeval time;
            gettimeofday(&time, NULL);
            
            time_t t = time.tv_sec;
            const double julianDay = toJulian(gmtime(static_cast<time_t *> (&t))->tm_year + 1900,
                                              gmtime(static_cast<time_t *> (&t))->tm_mon + 1,
                                              gmtime(static_cast<time_t *> (&t))->tm_mday,
                                              gmtime(static_cast<time_t *> (&t))->tm_hour,
                                              gmtime(static_cast<time_t *> (&t))->tm_min,
                                              gmtime(static_cast<time_t *> (&t))->tm_sec);
            options->setTime(julianDay);
        }
        
        if (origin_file)
        {
            if (options->InterpolateOriginFile())
            {
                // interpolate positions in the origin file to the
                // current time
                double thisRad, thisLat, thisLon, thisLocalTime = -1;
                interpolateOriginFile(options->getJulianDay(), 
                                      originVector, thisRad, 
                                      thisLat, thisLon, thisLocalTime);
                options->setRange(thisRad);
                options->Latitude(thisLat);
                options->Longitude(thisLon);
                options->LocalTime(thisLocalTime);
            }
            else
            {
                // Use the next time and position in the origin file
                options->setTime(iterOriginVector->time);
                options->setRange(iterOriginVector->radius);
                options->Latitude(iterOriginVector->latitude);
                options->Longitude(iterOriginVector->longitude);
                options->LocalTime(iterOriginVector->localTime);
            }
        }

        if (!options->DynamicOrigin().empty())
        {
            LBRPoint originPoint;
            readDynamicOrigin(options->DynamicOrigin(), originPoint);
            options->setRange(originPoint.radius);
            options->Latitude(originPoint.latitude);
            options->Longitude(originPoint.longitude);
            options->LocalTime(originPoint.localTime);
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
                       options->LightTime(), planetsFromSunMap);

        // Find the target body in the list
        body target_body = options->getTarget();
        Planet *target = findPlanetinMap(planetsFromSunMap, target_body);

        // LOOKAT mode is where the target isn't a planetary body.
        // (e.g. the Cassini spacecraft)
        if (options->TargetMode() == LOOKAT)
        {
            if (options->LightTime()) options->setTarget(planetProperties);
        }
        else
        {
            if (target == NULL)
                xpExit("Can't find target body?\n", __FILE__, __LINE__);

            if (options->LightTime())
            {
                double tX, tY, tZ;
                target->getPosition(tX, tY, tZ);
                options->setTarget(tX, tY, tZ);
            }

            // Find the sub-observer lat & lon
            double obs_lat, obs_lon;
            target->XYZToPlanetographic(oX, oY, oZ, obs_lat, obs_lon);
            options->Latitude(obs_lat);
            options->Longitude(obs_lon);

            // Find the sub-solar lat & lon.  This is used for the image
            // label
            double sunLat, sunLon;
            target->XYZToPlanetographic(0, 0, 0, sunLat, sunLon);
            options->SunLat(sunLat);
            options->SunLon(sunLon);
        }

        // delete the markerbounds file, since we'll create a new one
        string markerBounds(options->MarkerBounds());
        if (!markerBounds.empty())
            unlinkFile(markerBounds.c_str());

        // Initialize display device
        DisplayBase *display = getDisplay(times_run);

        if (options->ProjectionMode() == MULTIPLE)
        {
            drawMultipleBodies(display, target, planetsFromSunMap,
                               planetProperties);
        }
        else
        {
            drawProjection(display, target, planetsFromSunMap, 
                           planetProperties[target->Index()]);
        }

        display->renderImage(planetProperties);
        delete display;

        destroyPlanetMap();

        times_run++;

        if (!options->PostCommand().empty())
        {
            if (system(options->PostCommand().c_str()) != 0)
            {
                ostringstream errStr;
                errStr << "Can't execute " << options->PostCommand()
                       << "\n";
                xpWarn(errStr.str(), __FILE__, __LINE__);
            }
        }

        if (options->NumTimes() > 0 && times_run >= options->NumTimes())
            break;

        if (origin_file && !options->InterpolateOriginFile())
        {
            // If we've run through the origin file, break out of the
            // while(1) loop.
            iterOriginVector++;
            if (iterOriginVector == originVector.end()) break;
        }

        if (!options->UseCurrentTime())
        {
            // Set the time to the next update
            options->incrementTime((time_t) (options->getTimeWarp() 
                                             * options->getWait()));
        }

        // Sleep until the next update.  If Sleep() returns false,
        // then quit.
        if (!timer->Sleep()) break;
    }

#ifdef HAVE_CSPICE
    // unload any SPICE kernels
    processSpiceKernels(false);
#endif

    delete timer;

    for (int i = 0; i < RANDOM_BODY; i++) delete planetProperties[i];

    cleanUpEphemeris();

    return(EXIT_SUCCESS);
}
