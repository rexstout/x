#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

#include "keywords.h"
#include "Options.h"
#include "PlanetProperties.h"
#include "xpUtil.h"

#include "DisplayMSWin.h"

#include "libimage/Image.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

DisplayMSWin::DisplayMSWin(const int tr) : DisplayBase(tr)
{
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);

    Options *options = Options::getInstance();
    switch (options->getDisplayMode())
    {
    case WINDOW:
        xpWarn("-window option not supported for MS Windows.\n",
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

DisplayMSWin::~DisplayMSWin()
{
}

void 
DisplayMSWin::renderImage(PlanetProperties *planetProperties[])
{
    drawLabel(planetProperties);

    string outputFilename(TmpDir());
    outputFilename += "\\XPlanet.bmp";

    Image i(width_, height_, rgb_data, alpha);
    if (!i.Write(outputFilename.c_str()))
    {
        stringstream errStr;
        errStr << "Can't create image file " << outputFilename << "\n";
        xpExit(errStr.str(), __FILE__, __LINE__);
    }
    else
    {
	Options *options = Options::getInstance();
	if (options->Verbosity() > 1)
	{
	    stringstream msg;
	    msg << "Created image file " << outputFilename << "\n";
	    xpMsg(msg.str(), __FILE__, __LINE__);
	}
    }
    
    // Tell Windows to update the desktop wallpaper
    SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, 
                         (char *) outputFilename.c_str(), 
                         SPIF_UPDATEINIFILE);
}

string
DisplayMSWin::TmpDir()
{
    Options *options = Options::getInstance();

    string returnstring = options->TmpDir();
    if (returnstring.empty())
    {
        char tmpdir[MAX_PATH];
        GetWindowsDirectory(tmpdir, MAX_PATH);
        returnstring.assign(tmpdir);
    }
    return(returnstring);
}
