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
	options->setCenterX(width_/2);
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

    const char * const outputFilename = constructOutputFilename();

    Image i(width_, height_, rgb_data, alpha);
    i.Quality(quality_);
    if (!i.Write(outputFilename))
    {
	ostringstream errStr;
	errStr << "Can't create " << outputFilename << ".\n";
	xpExit(errStr.str(), __FILE__, __LINE__);
    }

    Options *options = Options::getInstance();
    if (options->Verbosity() > 1)
    {
	ostringstream msg;
	msg << "Created image file " << outputFilename << "\n";
	xpMsg(msg.str(), __FILE__, __LINE__);
    }
}

const char *
DisplayOutput::constructOutputFilename()
{
    Options *options = Options::getInstance();

    string output_filename = options->getOutputBase();
    if (options->NumTimes() > 1)
    {
	const int digits = (int) (log10((double) options->NumTimes()) + 1);
	char buffer[64];
	snprintf(buffer, 64, "%.*d", digits, times_run);
	output_filename += buffer;
    }
    output_filename += options->getOutputExtension();

    return(output_filename.c_str());
}
