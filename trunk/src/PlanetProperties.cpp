#include <algorithm>
#include <cctype>
#include <string>
#include <vector>
using namespace std;

#include "body.h"
#include "PlanetProperties.h"
#include "xpDefines.h"

PlanetProperties::PlanetProperties(const body index)
    : index_(index), 
      cloudGamma_(1),
      cloudMap_(""), 
      cloudThreshold_(90), 
      delOrbit_(2),
      drawOrbit_(false),
      grid_(false), 
      grid1_(6),
      grid2_(15),
      magnify_(1.0), 
      mapBounds_(false), 
      mapUlx_(0), mapUly_(0), mapLrx_(0), mapLry_(0),
      markerFont_(""),
      minRadiusForLabel_(.01),
      maxRadiusForLabel_(3.0),
      minRadiusForMarkers_(40.0), 
      nightMap_(""), 
      randomOrigin_(true),
      randomTarget_(true),
      shade_(0.3),
      specularMap_(""),
      startOrbit_(-0.5), stopOrbit_(0.5),
      twilight_(6)
{
    memset(arcColor_, 255, 3);              // default arc color is white
    memset(color_, 255, 3);
    memset(gridColor_, 255, 3);             // default grid color is white
    memset(markerColor_, 0, 3);
    memset(orbitColor_, 255, 3);
    memset(textColor_, 0, 3);

    markerColor_[0] = 255;                  // default marker color is red
    textColor_[0] = 255;                    // default text color is red

    arcFiles_.clear();
    markerFiles_.clear();
    satelliteFiles_.clear();

    if (index < RANDOM_BODY) 
    {
        name_ = body_string[index];
        dayMap_ = name_ + defaultMapExt;
        name_[0] = toupper(name_[0]);
    }

    if (index == SUN)
        color_[2] = 166;

    if (index == EARTH)
        nightMap_ = "night" + defaultMapExt;
}

PlanetProperties::~PlanetProperties()
{
}

PlanetProperties &
PlanetProperties::operator= (const PlanetProperties &p)
{
    memcpy(arcColor_, p.arcColor_, 3);
    memcpy(color_, p.color_, 3);
    memcpy(gridColor_, p.gridColor_, 3);
    memcpy(markerColor_, p.markerColor_, 3);
    memcpy(orbitColor_, p.orbitColor_, 3);
    memcpy(textColor_, p.textColor_, 3);

    for (unsigned int i = 0; i < p.arcFiles_.size(); i++)
        arcFiles_.push_back(p.arcFiles_[i]);

    for (unsigned int i = 0; i < p.markerFiles_.size(); i++)
        markerFiles_.push_back(p.markerFiles_[i]);

    for (unsigned int i = 0; i < p.satelliteFiles_.size(); i++)
        satelliteFiles_.push_back(p.satelliteFiles_[i]);

    cloudGamma_ = p.cloudGamma_;
    cloudMap_ = p.cloudMap_;

    delOrbit_ = p.delOrbit_;
    drawOrbit_ = p.drawOrbit_;

    grid_ = p.grid_;
    grid1_ = p.grid1_;
    grid2_ = p.grid2_;

    magnify_ = p.magnify_;

    mapBounds_ = p.mapBounds_;
    mapUly_ = p.mapUly_;
    mapUlx_ = p.mapUlx_;
    mapLry_ = p.mapLry_;
    mapLrx_ = p.mapLrx_;

    markerFont_ = p.markerFont_;

    minRadiusForLabel_ = p.minRadiusForLabel_;
    maxRadiusForLabel_ = p.maxRadiusForLabel_;

    minRadiusForMarkers_ = p.minRadiusForMarkers_;

    randomOrigin_ = p.randomOrigin_;
    randomTarget_ = p.randomTarget_;
    
    shade_ = p.shade_;

    startOrbit_ = p.startOrbit_;
    stopOrbit_ = p.stopOrbit_;

    return(*this);
}
