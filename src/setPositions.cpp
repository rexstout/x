#include <map>
using namespace std;

#include "buildPlanetMap.h"
#include "findBodyXYZ.h"
#include "keywords.h"
#include "PlanetProperties.h"
#include "Options.h"
#include "readOriginFile.h"
#include "xpUtil.h"

#include "libplanet/Planet.h"

// if an origin file is being used, set the observer position from it
void
setOriginXYZFromFile(const vector<LBRPoint> &originVector, 
                     const vector<LBRPoint>::iterator &iterOriginVector)
{
    Options *options = Options::getInstance();

    // if an origin file is specified, set observer position from it
    if (!options->OriginFile().empty())
    {
        if (options->InterpolateOriginFile())
        {
            // interpolate positions in the origin file to the
            // current time
            double thisRad, thisLat, thisLon, thisLocalTime = -1;
            interpolateOriginFile(options->JulianDay(), 
                                  originVector, thisRad, 
                                  thisLat, thisLon, thisLocalTime);
            options->Range(thisRad);
            options->Latitude(thisLat);
            options->Longitude(thisLon);
            options->LocalTime(thisLocalTime);
        }
        else
        {
            // Use the next time in the origin file
            options->setTime(iterOriginVector->time);

            // Use the position, if specified, in the origin file
            if (iterOriginVector->radius > 0)
            {
                options->Range(iterOriginVector->radius);
                options->Latitude(iterOriginVector->latitude);
                options->Longitude(iterOriginVector->longitude);

                // Use the local time, if specified, in the origin file
                if (iterOriginVector->localTime > 0)
                    options->LocalTime(iterOriginVector->localTime);
            }
        }
    }

    // if -dynamic_origin was specified, set observer position
    // from it.
    if (!options->DynamicOrigin().empty())
    {
        LBRPoint originPoint;
        readDynamicOrigin(options->DynamicOrigin(), originPoint);
        options->Range(originPoint.radius);
        options->Latitude(originPoint.latitude);
        options->Longitude(originPoint.longitude);
        options->LocalTime(originPoint.localTime);
    }
}

// set the position of the target.  The coordinates are stored in the
// Options class.
void
setTargetXYZ(PlanetProperties *planetProperties[])
{
    Options *options = Options::getInstance();
    options->setTarget(planetProperties);
}

// in most cases, this is really where the origin gets set
void
setOriginXYZ(PlanetProperties *planetProperties[])
{
    Options *options = Options::getInstance();
    options->setOrigin(planetProperties);
}

// Set the observer's lat/lon with respect to the target body, if
// appropriate
void
getObsLatLon(Planet *target, PlanetProperties *planetProperties[])
{
    Options *options = Options::getInstance();

    // XYZ mode is where the target isn't a planetary body.
    // (e.g. the Cassini spacecraft)
    if (options->TargetMode() == XYZ)
    {
        if (options->LightTime()) 
            options->setTarget(planetProperties);
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

        // Rectangular coordinates of the observer
        double oX, oY, oZ;
        options->getOrigin(oX, oY, oZ);

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
}

// Set the direction of the "Up" vector
void
setUpXYZ(const Planet *target, map<double, Planet *> &planetsFromSunMap,
         double &upX, double &upY, double &upZ)
{
    Options *options = Options::getInstance();

    switch (options->North())
    {
    default:
        xpWarn("Unknown value for north, using body\n", 
               __FILE__, __LINE__);
    case BODY:
        target->getBodyNorth(upX, upY, upZ);
        break;
    case GALACTIC:
        target->getGalacticNorth(upX, upY, upZ);
        break;
    case ORBIT:
    {
        // if it's a moon, pretend its orbital north is the same as
        // its primary, although in most cases it's the same as its
        // primary's rotational north
        if (target->Primary() == SUN)
        {
            target->getOrbitalNorth(upX, upY, upZ);
        }
        else
        {
            Planet *primary = findPlanetinMap(planetsFromSunMap, 
                                              target->Primary());
            primary->getOrbitalNorth(upX, upY, upZ);
        }
    }
    break;
    case PATH:
    {
        double vX, vY, vZ;
        findBodyVelocity(options->JulianDay(), 
                         options->getOrigin(), 
                         options->OriginID(),
                         options->PathRelativeTo(), 
                         options->PathRelativeToID(), 
                         vX, vY, vZ);
            
        upX = vX * FAR_DISTANCE;
        upY = vY * FAR_DISTANCE;
        upZ = vZ * FAR_DISTANCE;
    }
    break;
    case TERRESTRIAL:
        upX = 0;
        upY = 0;
        upZ = FAR_DISTANCE;
        break;
    }

}
