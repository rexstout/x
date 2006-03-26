#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <sstream>
using namespace std;

#include "keywords.h"
#include "Options.h"
#include "PlanetProperties.h"
#include "xpUtil.h"

#include "DisplayOutput.h"

#include "libimage/Image.h"

DisplayOutput::DisplayOutput(const int tr) : DisplayBase(tr)
{
    Options *options = Options::getInstance();
    width_ = options->getWidth();
    height_ = options->getHeight();

    quality_ = options->JPEGQuality();

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

DisplayOutput::~DisplayOutput()
{
}

void
DisplayOutput::renderImage(PlanetProperties *planetProperties[])
{
    drawLabel(planetProperties);

    Options *options = Options::getInstance();
    string outputFilename = options->getOutputBase();
    if (options->NumTimes() > 1)
    {
        const int digits = (int) (log10((double) options->NumTimes()) + 1);
        char buffer[64];
        snprintf(buffer, 64, "%.*d", digits, times_run);
        outputFilename += buffer;
    }
    outputFilename += options->getOutputExtension();

    Image i(width_, height_, rgb_data, alpha);
    i.Quality(quality_);
    if (!i.Write(outputFilename.c_str()))
    {
        ostringstream errStr;
        errStr << "Can't create " << outputFilename << ".\n";
        xpExit(errStr.str(), __FILE__, __LINE__);
    }

    if (options->Verbosity() > 1)
    {
        ostringstream msg;
        msg << "Created image file " << outputFilename << "\n";
        xpMsg(msg.str(), __FILE__, __LINE__);
    }
}
