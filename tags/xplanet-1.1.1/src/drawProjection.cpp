#include <map>
#include <vector>
using namespace std;

#include "buildPlanetMap.h"
#include "config.h"
#include "createMap.h"
#include "keywords.h"
#include "Map.h"
#include "Options.h"
#include "PlanetProperties.h"
#include "Ring.h"
#include "satrings.h"
#include "sphericalToPixel.h"
#include "xpUtil.h"

#include "libannotate/libannotate.h"
#include "libdisplay/libdisplay.h"
#include "libplanet/Planet.h"
#include "libprojection/libprojection.h"
#include "libprojection/ProjectionRectangular.h"

extern void
arrangeMarkers(multimap<double, Annotation *> &annotationMap,
               DisplayBase *display);

void
drawProjection(DisplayBase *display, Planet *target,
               const double upX, const double upY, const double upZ, 
               map<double, Planet *> &planetsFromSunMap,
               PlanetProperties *planetProperties)
{
    const int height = display->Height();
    const int width = display->Width();

    // subsolar lat/lon
    double sLat, sLon;
    target->XYZToPlanetographic(0, 0, 0, sLat, sLon);

    // lat/lon of the "up" vector
    double nLat, nLon;
    target->XYZToPlanetographic(upX * FAR_DISTANCE, 
                                upY * FAR_DISTANCE, 
                                upZ * FAR_DISTANCE, 
                                nLat, nLon);

    // Rotate the image so that the "up" vector points to the top of
    // the screen
    Options *options = Options::getInstance();

    double tc, dist;
    calcGreatArc(options->Latitude(), 
                 options->Longitude() * target->Flipped(),
                 nLat, nLon * target->Flipped(), tc, dist);
    options->Rotate(-tc);

    Ring *ring = NULL;
    if (target->Index() == SATURN)
    {
        double X, Y, Z;
        target->getPosition(X, Y, Z);
        ring = new Ring(inner_radius, outer_radius, saturn_radius, 
                        ring_brightness, LIT, ring_transparency, TRANSP,
                        sLon, sLat, 
                        planetProperties->Shade(), 
                        target);
    }

    const bool isSun = (target->Index() == SUN);
    Map *m = NULL;
    if (!isSun)
        m = createMap(sLat, sLon,
                      options->Latitude(), options->Longitude(), 
                      width, height, options->Radius() * height,
                      target, ring, planetsFromSunMap,
                      planetProperties);

    if (target->Index() == SATURN) 
    {
        delete ring;
    }

    ProjectionBase *projection = NULL;

    if (options->ProjectionMode() == RANDOM)
        options->Projection(getRandomProjection());
    else
        options->Projection(options->ProjectionMode());

    if (options->Projection() == RECTANGULAR
        && planetProperties->MapBounds())
    {
        projection = new ProjectionRectangular(target->Flipped(), 
                                               width, height,
                                               m->StartLat(),
                                               m->StartLon(),
                                               m->MapHeight(),
                                               m->MapWidth());
                                               
    }
    else
    {
        projection = getProjection(options->Projection(),
                                   target->Flipped(), 
                                   width, height);
    }

    multimap<double, Annotation *> annotationMap;

#ifdef HAVE_CSPICE
    if (!options->SpiceFiles().empty())
        addSpiceObjects(planetsFromSunMap, NULL, projection, annotationMap);
#endif

    if (planetProperties->DrawArcs())
        addArcs(planetProperties, target, NULL, projection, 
                annotationMap);

    if (planetProperties->DrawMarkers())
        addMarkers(planetProperties, target, 0, 0, 0, 0, 
                   NULL, projection, width, height, 
                   planetsFromSunMap, annotationMap);

    if (planetProperties->DrawSatellites())
        addSatellites(planetProperties, target, NULL, projection,
                      annotationMap);

    const bool limbDarkening = (options->Projection() == HEMISPHERE
                                || options->Projection() == ORTHOGRAPHIC);

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            double lon, lat;
            if (projection->pixelToSpherical(i, j, lon, lat))
            {
                unsigned char color[3];
                if (isSun) 
                    memcpy(color, planetProperties->Color(), 3);
                else
                    m->GetPixel(lat, lon * target->Flipped(), color);

                if (limbDarkening)
                {
                    for (int i = 0; i < 3; i++) 
                        color[i] = (unsigned char) 
                            (color[i] * projection->getDarkening());
                }
                display->setPixel(i, j, color);
            }
        }
    }

    if (planetProperties->Grid())
    {
        const double grid1 = planetProperties->Grid1();
        const double grid2 = planetProperties->Grid2();
        const unsigned char *color = planetProperties->GridColor();
        for (double lat = -M_PI_2; lat <= M_PI_2; lat += M_PI_2/grid1)
        {
            for (double lon = -M_PI; lon <= M_PI; lon += M_PI_2/(grid1 * grid2))
            {
                double X, Y, Z;
                if (sphericalToPixel(lat, lon, 1, X, Y, Z, target, 
                                     NULL, projection)) 
                    display->setPixel(X, Y, color);
            }
        }

        for (double lat = -M_PI_2; lat <= M_PI_2; lat += M_PI_2/(grid1 * grid2))
        {
            for (double lon = -M_PI; lon <= M_PI; lon += M_PI_2/grid1)
            {
                double X, Y, Z;
                if (sphericalToPixel(lat, lon, 1, X, Y, Z, target, 
                                     NULL, projection)) 
                    display->setPixel(X, Y, color);
            }
        }
    }

    if (!annotationMap.empty())
    {
        multimap<double, Annotation *>::iterator annotationIterator;

        // place markers so that they don't overlap one another, if
        // possible
        arrangeMarkers(annotationMap, display);
        
        for (annotationIterator = annotationMap.begin(); 
             annotationIterator != annotationMap.end(); 
             annotationIterator++)
        {
            Annotation *a = annotationIterator->second;
            a->Draw(display);
            // if the projection "wraps around", add annotations on
            // the either side of the screen.
            if (projection->IsWrapAround() 
                && !planetProperties->MapBounds())
            {
                a->Shift(-width);
                a->Draw(display);
                a->Shift(2*width);
                a->Draw(display);
            }

            delete annotationIterator->second;
        }
    }

    delete m;

    delete projection;
}
