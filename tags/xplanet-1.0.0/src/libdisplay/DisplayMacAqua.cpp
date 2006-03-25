#include <cstdlib>
#include <cstring>
#include <sstream>
using namespace std;

#include "keywords.h"
#include "Options.h"
#include "PlanetProperties.h"
#include "xpUtil.h"

#include "DisplayMacAqua.h"

#include "libimage/Image.h"

DisplayMacAqua::DisplayMacAqua(const int tr) : DisplayBase(tr)
{
    using namespace Aqua;

    int screen_width = (int) CGDisplayPixelsWide(kCGDirectMainDisplay);
    int screen_height = (int) CGDisplayPixelsHigh(kCGDirectMainDisplay);

    //    int screen_depth = (int) CGDisplayBitsPerPixel(kCGDirectMainDisplay);

    Options *options = Options::getInstance();
    switch (options->getDisplayMode())
    {
    case WINDOW:
        xpWarn("-window option not supported for Aqua.\n", 
               __FILE__, __LINE__);
        // fall through
    case ROOT:
        width_ = screen_width;
        height_ = screen_height;
        break;
    }
  
    if (!options->CenterSelected())
    {
        if (width_ % 2 == 0)
            options->setCenterX(width_/2 - 0.5);
        else
            options->setCenterX(width_/2);

        if (height_ % 2 == 0)
            options->setCenterY(height_/2 - 0.5);
        else
            options->setCenterY(height_/2);
    }

    allocateRGBData();

}

DisplayMacAqua::~DisplayMacAqua()
{
}

void 
DisplayMacAqua::renderImage(PlanetProperties *planetProperties[])
{
}
