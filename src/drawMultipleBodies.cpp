#include <sys/time.h>

#include <cstdio>
#include <ctime>
#include <map>
#include <sstream>
#include <vector>
using namespace std;

#include "body.h"
#include "buildPlanetMap.h"
#include "createMap.h"
#include "findFile.h"
#include "keywords.h"
#include "Map.h"
#include "Options.h"
#include "PlanetProperties.h"
#include "Ring.h"
#include "satrings.h"
#include "sphericalToPixel.h"
#include "View.h"
#include "xpUtil.h"

#include "libannotate/libannotate.h"
#include "libdisplay/libdisplay.h"
#include "libimage/Image.h"
#include "libmultiple/libmultiple.h"
#include "libplanet/Planet.h"

struct plotDetail
{
    Planet *p;
    double X, Y;              // pixel location
    double radius;            // radius in pixels
    double sun_lat, sun_lon;  // sub-solar location
    double obs_lat, obs_lon;  // sub-observer location
};

extern void
arrangeMarkers(multimap<double, Annotation *> &annotationMap,
               DisplayBase *display);

void
drawMultipleBodies(DisplayBase *display, Planet *target, 
                   map<double, Planet *> &planetsFromSunMap,
                   PlanetProperties *planetProperties[])
{
    Options *options = Options::getInstance();

    const int width = display->Width();
    const int height = display->Height();

    // Get the rectangular position of the origin
    double oX, oY, oZ;
    options->getOrigin(oX, oY, oZ);

    // Get the rectangular position of the target
    double tX, tY, tZ;
    options->getTarget(tX, tY, tZ);

    // Get the displacement from origin to target
    double dX = tX - oX;
    double dY = tY - oY;
    double dZ = tZ - oZ;
    double target_dist = sqrt(dX*dX + dY*dY + dZ*dZ);
    if (target_dist == 0)
        xpExit("Origin and target bodies can't be the same!\n",
               __FILE__, __LINE__);

    if (options->Verbosity() > 0)
    {
        if (options->OriginMode() == BODY)
        {
            stringstream msg;
            msg << "Looking at "
                << planetProperties[options->getTarget()]->Name()
                << " from "
                << planetProperties[options->getOrigin()]->Name()
                << endl;
            xpMsg(msg.str(), __FILE__, __LINE__);
        }
        if (options->Verbosity() > 1)
        {
            char buffer[128];
            snprintf(buffer, 128, 
                     "target dist = %12.6f, in units of radius = %12.6f\n", 
                     target_dist,
                     target_dist / (planetProperties[target->Index()]->Magnify()
                                    * target->Radius()));
            xpMsg(buffer, __FILE__, __LINE__);
        }
    }
    // Find the pixel radius and angle subtended by the target.
    // This is used to get degrees per pixel.
    const double target_angular_radius = target->Radius() / target_dist;
    double target_pixel_radius;
    double pixels_per_radian;

    switch (options->FOVMode())
    {
    case RADIUS:
        target_pixel_radius = (options->Radius() * height);
        target_pixel_radius /= planetProperties[options->getTarget()]->Magnify();

        if (target->Index() == SATURN) target_pixel_radius /= 2.32166;

        pixels_per_radian = target_pixel_radius / target_angular_radius;
        options->FieldOfView(width / pixels_per_radian);
        break;
    case FOV:
    {
        pixels_per_radian = width / options->FieldOfView();
        target_pixel_radius = target_angular_radius * pixels_per_radian;

        double scale = 1.0;
        if (target->Index() == SATURN) scale *= 2.32166;  

        options->Radius(scale * target_pixel_radius / height);
    }
    break;
    default:
        xpExit("drawMultipleBodies: Unknown FOV mode???\n", __FILE__, 
               __LINE__);
    }

    // Linear distance per pixel
    const double dist_per_pixel = target->Radius()/target_pixel_radius;

    if (options->Verbosity() > 1)
    {
        stringstream msg;
        char buffer[128];
        snprintf(buffer, 128, "fov = %14.8f degrees\n", 
                 options->FieldOfView()/deg_to_rad);
        msg << buffer;
        snprintf(buffer, 128, "dist_per_pixel = %14.8f\n", dist_per_pixel);
        msg << buffer;
        xpMsg(msg.str(), __FILE__, __LINE__);
    }

    // Set the view matrix
    double upX, upY, upZ;
    switch (options->North())
    {
    default:
        xpWarn("Unknown value for north, using body\n", __FILE__, __LINE__);
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
    case TERRESTRIAL:
        upX=0;
        upY=0;
        upZ=1;
        break;
    }

    // Put the primary in the center of the field of view
    if (options->OriginMode() == ABOVE
        || options->OriginMode() == BELOW)
    {
        Planet *primary = findPlanetinMap(planetsFromSunMap, 
                                          target->Primary());
        primary->getPosition(tX, tY, tZ);
    }

    View *view = new View(oX, oY, oZ, 
                          tX, tY, tZ, 
                          upX*1e6, upY*1e6, upZ*1e6, 
                          dist_per_pixel, options->getRotate());

    multimap<double, Annotation *> annotationMap;
    annotationMap.clear();

    multimap<double, plotDetail> planetMap;
    planetMap.clear();

    // Now run through all of the other bodies to see if any of
    // them are in the field of view
    for (map<double, Planet *>::iterator it0 = planetsFromSunMap.begin(); 
         it0 != planetsFromSunMap.end(); it0++)
    {
        Planet *current_planet = it0->second;
        body b = current_planet->Index();

        PlanetProperties *currentProperties = planetProperties[current_planet->Index()];

        // Get the planet's position
        double pX, pY, pZ;
        current_planet->getPosition(pX, pY, pZ);
        
        // Now get the position relative to the origin
        dX = pX - oX;
        dY = pY - oY;
        dZ = pZ - oZ;
        double dist = sqrt(dX*dX + dY*dY + dZ*dZ);
        
        if (currentProperties->DrawOrbit())
        {
            double light_time = 0;
            if (options->LightTime())
            {
                light_time = dist * AU_to_km / 299792.458;
                light_time /= 86400;
            }

            addOrbits(options->getJulianDay() - light_time, 
                      view, width, height, 
                      current_planet, 
                      currentProperties,
                      annotationMap);
        }

        if (dist == 0) continue;
        const double angularRadius = current_planet->Radius() / dist;
        double pixel_radius = (angularRadius * pixels_per_radian 
                               * currentProperties->Magnify());
        
        // Get the pixel location of this body
        double X, Y, Z;
        view->XYZToPixel(pX, pY, pZ, X, Y, Z);
        X += options->getCenterX();
        Y += options->getCenterY();

        // only annotate the image if it's big enough
        if (currentProperties->MinRadiusForMarkers() < pixel_radius)
        {
            if (currentProperties->DrawArcs())
                addArcs(currentProperties, current_planet, view, NULL, 
                        annotationMap);
            
            if (currentProperties->DrawMarkers())
                addMarkers(currentProperties, current_planet, X, Y, Z, 
                           view, NULL, width, height, 
                           planetsFromSunMap, annotationMap);
            
            if (currentProperties->DrawSatellites())
                addSatellites(currentProperties, current_planet, view, NULL, 
                              annotationMap);
        }

        // Even if the disk of the Sun or Saturn is off the
        // screen, we still want to draw the glare or the rings.
        if (b == SUN) 
            pixel_radius *= 28;
        else if (b == SATURN) 
            pixel_radius *= 2.32166;

        // if it's behind us or off the screen, skip this one
        if (Z < 0 
            || X < -pixel_radius || X > width + pixel_radius 
            || Y < -pixel_radius || Y > height + pixel_radius) continue;
        
        if (b == SUN) 
            pixel_radius /= 28;
        else if (b == SATURN) 
            pixel_radius /= 2.32166;
        
        // Now calculate the sub-solar and sub-observer points
        double sun_lat, sun_lon;
        current_planet->XYZToPlanetographic(0, 0, 0, sun_lat, sun_lon);
        
        double obs_lat, obs_lon;
        current_planet->XYZToPlanetographic(oX, oY, oZ, obs_lat, obs_lon);
        
        // Label this body with its name
        if (pixel_radius >= currentProperties->MinRadiusForLabel() 
            && pixel_radius <= currentProperties->MaxRadiusForLabel())
        {
            Text *t = new Text(currentProperties->TextColor(),
                               static_cast<int> (X + 0.5), 
                               static_cast<int> (Y + 0.5), 
                               static_cast<int> (pixel_radius + 1), 
                               static_cast<int> (pixel_radius + 1), 
                               AUTO, currentProperties->Name());

            annotationMap.insert(pair<const double, Annotation*>(Z, t));
        }

        // And save for plotting
        plotDetail planetDetail = {current_planet, X, Y, pixel_radius, 
                                   sun_lat, sun_lon, obs_lat, obs_lon};
        planetMap.insert(pair<const double, plotDetail>(Z, planetDetail));
    }

    if (options->Verbosity() > 0)
    {
        stringstream msg;
        char buffer[256];
        snprintf(buffer, 256, "%10s%14s%14s%14s%14s%14s%14s%14s%14s\n", 
                 "Name", "Dist", "X", "Y", "radius",
                 "subsolar lat", "subsolar lon", 
                 "observer lat", "observer lon");
        msg << buffer;
        xpMsg(msg.str(), __FILE__, __LINE__);
    }

    drawStars(display, view);

    if (!options->ArcFiles().empty())
        addArcs(view, annotationMap);

    if (!options->MarkerFiles().empty())
        addMarkers(view, width, height, planetsFromSunMap, annotationMap);

    // place markers so that they don't overlap one another, if
    // possible
    arrangeMarkers(annotationMap, display);

    // draw all of the annotations
    if (!annotationMap.empty())
    {
        multimap<double, Annotation *>::iterator annotationIterator;
        for (annotationIterator = annotationMap.begin(); 
             annotationIterator != annotationMap.end(); 
             annotationIterator++)
        {
            (annotationIterator->second)->Draw(display);
        }
    }

    // planetMap contains a list of bodies to plot, sorted by
    // distance to the observer
    multimap<double, plotDetail>::iterator planetIterator = planetMap.end();
    while (planetIterator != planetMap.begin())
    {
        planetIterator--;
        const double dist_to_planet = planetIterator->first;
        Planet *current_planet = planetIterator->second.p;
        const double pX = planetIterator->second.X;
        const double pY = planetIterator->second.Y;
        double pR = planetIterator->second.radius;
        const double sLat = planetIterator->second.sun_lat;
        const double sLon = planetIterator->second.sun_lon;
        const double oLat = planetIterator->second.obs_lat;
        const double oLon = planetIterator->second.obs_lon;

        PlanetProperties *currentProperties = planetProperties[current_planet->Index()];

        if (options->Verbosity() > 0)
        {
            stringstream msg;
            char buffer[256];
            snprintf(buffer, 256, 
                     "%10s%14.4f%14.4f%14.4f%14.4f%14.4f%14.4f%14.4f%14.4f\n", 
                     currentProperties->Name().c_str(), 
                     dist_to_planet, pX, pY, pR, 
                     sLat/deg_to_rad, sLon/deg_to_rad, 
                     oLat/deg_to_rad, oLon/deg_to_rad);
            msg << buffer;
            xpMsg(msg.str(), __FILE__, __LINE__);
        }

        if (pR <= 1)
        {
            display->setPixel(static_cast<int> (pX + 0.5), 
                              static_cast<int> (pY + 0.5), 
                              currentProperties->Color());
            if (current_planet->Index() == SUN) 
                drawSun(display, pX, pY, 1, currentProperties->Color());

            continue;
        }

        if (current_planet->Index() == SUN) 
        {
            drawSun(display, pX, pY, pR, currentProperties->Color());
        }
        else
        {
            double X, Y, Z;
            current_planet->getPosition(X, Y, Z);
        
            Ring *ring = NULL;
            // Draw the far side of Saturn's rings
            if (current_planet->Index() == SATURN) 
            {
                ring = new Ring(inner_radius, outer_radius, saturn_radius, 
                                ring_brightness, LIT, ring_transparency, 
                                TRANSP, sLon, sLat,
                                currentProperties->Shade(), 
                                current_planet);

                const bool lit_side = (sLat * oLat > 0);
                drawRings(current_planet, display, view, ring, pX, pY, pR, 
                          oLat, oLon, lit_side, true);
            }

            Map *m = NULL;
            m = createMap(sLat, sLon, oLat, oLon, width, height, pR,
                          current_planet, ring, planetsFromSunMap,
                          currentProperties);
            if (current_planet->Index() == JUPITER
                || current_planet->Index() == SATURN)
            {
                drawEllipsoid(pX, pY, pR, oX, oY, oZ, 
                              X, Y, Z, display, view, m,
                              current_planet, 
                              currentProperties->Magnify());
            }
            else
            {
                drawSphere(pX, pY, pR, oX, oY, oZ, 
                           X, Y, Z, display, view, m,
                           current_planet, 
                           current_planet->Radius() 
                           * currentProperties->Magnify());
            }
            delete m;

            // Draw the grid
            if (currentProperties->Grid())
            {
                const double grid1 = currentProperties->Grid1();
                const double grid2 = currentProperties->Grid2();
                const unsigned char *color = currentProperties->GridColor();
                for (double lat = -M_PI_2; lat <= M_PI_2; lat += M_PI_2/grid1)
                {
                    const double radius = current_planet->Radius(lat);

                    for (double lon = -M_PI; lon <= M_PI; lon += M_PI_2/(grid1 * grid2))
                    {
                        double X, Y, Z;
                        sphericalToPixel(lat, lon, 
                                         radius * currentProperties->Magnify(),
                                         X, Y, Z, current_planet, view, NULL);
                        if (Z < dist_to_planet) display->setPixel(X, Y, color);
                    }
                }
                
                for (double lat = -M_PI_2; lat <= M_PI_2; lat += M_PI_2/(grid1 * grid2))
                {
                    const double radius = current_planet->Radius(lat);

                    for (double lon = -M_PI; lon <= M_PI; lon += M_PI_2/grid1)
                    {
                        double X, Y, Z;
                        sphericalToPixel(lat, lon, 
                                         radius * currentProperties->Magnify(),
                                         X, Y, Z, current_planet, view, NULL);
                        if (Z < dist_to_planet) display->setPixel(X, Y, color);
                    }
                }
            }
            
            // Draw the near side of Saturn's rings
            if (current_planet->Index() == SATURN) 
            {
                const bool lit_side = (sLat * oLat > 0);
                drawRings(current_planet, display, view, ring, pX, pY, pR, 
                          oLat, oLon, lit_side, false);
            }

            delete ring;
        }

        // draw all of the annotations in front of this body
        if (!annotationMap.empty())
        {
            multimap<double, Annotation *>::iterator annotationIterator = annotationMap.begin();
            while (annotationIterator != annotationMap.end() 
                   && annotationIterator->first < dist_to_planet)
            {
                (annotationIterator->second)->Draw(display);
                annotationIterator++;
            }
        }
    }

    // clean up the annotationMap
    if (!annotationMap.empty())
    {
        multimap<double, Annotation *>::iterator annotationIterator;
        for (annotationIterator = annotationMap.begin(); 
             annotationIterator != annotationMap.end(); 
             annotationIterator++)
            delete annotationIterator->second;
    }

    delete view;
}
