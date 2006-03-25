#ifndef DISPLAYMACAQUA_H
#define DISPLAYMACAQUA_H

#include "DisplayBase.h"

namespace Aqua
{
#include <ApplicationServices/ApplicationServices.h>
}

class DisplayMacAqua : public DisplayBase
{
 public:
    DisplayMacAqua(const int tr);
    virtual ~DisplayMacAqua();

    void renderImage(PlanetProperties *planetProperties[]);

 private:
    Aqua::CGContextRef context;
};

#endif
