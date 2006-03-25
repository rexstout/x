#include <map>
#include <vector>
using namespace std;

#include "buildPlanetMap.h"
#include "createMap.h"
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
#include "libplanet/Planet.h"
#include "libprojection/libprojection.h"

extern void
arrangeMarkers(multimap<double, Annotation *> &annotationMap,
	       DisplayBase *display);

void
drawProjection(DisplayBase *display, Planet *target,
	       map<double, Planet *> &planetsFromSunMap,
	       PlanetProperties *planetProperties)
{
    const int height = display->Height();
    const int width = display->Width();

    double sLat, sLon;
    target->XYZToPlanetocentric(0, 0, 0, sLat, sLon);

    Options *options = Options::getInstance();

    double upX, upY, upZ;
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
	if (target->Primary() == SUN)
	    target->getOrbitalNorth(upX, upY, upZ);
	else
	{
	    const Planet *primary = findPlanetinMap(planetsFromSunMap, 
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

    double nLat, nLon;
    target->XYZToPlanetocentric(upX*1e6, upY*1e6, upZ*1e6, nLat, nLon);
    double tc, dist;
    calcGreatArc(options->Latitude(), 
		 options->Longitude() * target->Flipped(),
		 nLat, nLon * target->Flipped(), tc, dist);
    options->setRotate(-tc);

    View *sun_view = NULL;
    Ring *ring = NULL;
    if (target->Index() == SATURN)
    {
	double X, Y, Z;
	target->getPosition(X, Y, Z);
	sun_view = new View(0, 0, 0, X, Y, Z, 0, 0, 1e6, 
			    target->Radius());
	ring = new Ring(inner_radius, outer_radius, saturn_radius, 
			ring_brightness, LIT, ring_transparency, TRANSP,
			sLon, sLat, 
			planetProperties->Shade(), 
			target, sun_view);
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
	delete sun_view;
    }

    ProjectionBase *projection = getProjection(options->Projection(),
					       target->Flipped(), 
					       width, height);

    multimap<double, Annotation *> annotationMap;
    if (planetProperties->DrawArcs())
	addArcs(planetProperties, target, NULL, projection, 
		annotationMap);

    if (planetProperties->DrawMarkers())
	addMarkers(planetProperties, target, 0, 0, 0, 
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
		if (sphericalToPixel(lat, lon, 1, X, Y, Z, NULL, NULL, projection)) 
		    display->setPixel(X, Y, color);
	    }
	}

	for (double lat = -M_PI_2; lat <= M_PI_2; lat += M_PI_2/(grid1 * grid2))
	{
	    for (double lon = -M_PI; lon <= M_PI; lon += M_PI_2/grid1)
	    {
		double X, Y, Z;
		if (sphericalToPixel(lat, lon, 1, X, Y, Z, NULL, NULL, projection)) 
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
	    if (options->Projection() == MERCATOR
		|| options->Projection() == PETERS
		|| options->Projection() == RECTANGULAR)
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
