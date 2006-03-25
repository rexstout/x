#ifndef LIBANNOTATE_H
#define LIBANNOTATE_H

#include "Icon.h"
#include "LineSegment.h"
#include "Symbol.h"
#include "Text.h"

#include <map>

class Annotation;
class Planet;
class PlanetProperties;
class ProjectionBase;
class View;

extern void
addArcs(PlanetProperties *planetProperties, Planet *planet, 
	View *view, ProjectionBase *projection, 
	multimap<double, Annotation *> &annotationMap);

extern void
addMarkers(PlanetProperties *planetProperties, Planet *planet, 
	   const double X, const double Y, const double Z,
	   View *view, ProjectionBase *projection, 
	   const int width, const int height, 
	   map<double, Planet *> &planetsFromSunMap, 
	   std::multimap<double, Annotation *> &annotationMap);

extern void
addSatellites(PlanetProperties *planetProperties, Planet *planet, 
	      View *view, ProjectionBase *projection, 
	      std::multimap<double, Annotation *> &annotationMap);

extern void
loadSatelliteVector(PlanetProperties *planetProperties);

#endif
