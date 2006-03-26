#include <bitset>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

#include "body.h"
#include "findFile.h"
#include "keywords.h"
#include "Options.h"
#include "ParseGeom.h"
#include "PlanetProperties.h"
#include "xpDefines.h"
#include "xpUtil.h"

#include "DisplayBase.h"

#include "libimage/Image.h"

extern TextRenderer *getTextRenderer(DisplayBase *display);

DisplayBase::DisplayBase(const int tr) : times_run(tr)
{
    textRenderer_ = getTextRenderer(this);
}

DisplayBase::~DisplayBase()
{
    delete [] rgb_data;
    delete [] alpha;

    delete textRenderer_;
}

void
DisplayBase::Font(const string &fontname)
{
    textRenderer_->Font(fontname);
}

void
DisplayBase::FontSize(const int size)
{
    textRenderer_->FontSize(size);
}

void
DisplayBase::getTextBox(int &textWidth, int &textHeight)
{
    textRenderer_->TextBox(textWidth, textHeight);
}

// Remember to call FreeText() when done with the text operation
void
DisplayBase::setText(const string &text)
{
    textRenderer_->SetText(text);
}

void
DisplayBase::FreeText()
{
    textRenderer_->FreeText();
}

// x and y are the center left coordinates of the string
void 
DisplayBase::DrawOutlinedText(const int x, int y, const string &text, 
                              const unsigned char color[3])
{
    textRenderer_->DrawOutlinedText(x, y, text, color);
}

void
DisplayBase::drawLabelLine(int &currentX, int &currentY, const string &text)
{
    setText(text);
    
    int textWidth, textHeight;
    getTextBox(textWidth, textHeight);
    
    FreeText();

    Options *options = Options::getInstance();
    if (options->LabelMask() & XNegative) 
    {
        currentX = (options->LabelX() + width_ - textWidth);
    }

    DrawOutlinedText(currentX, currentY, text, options->Color());
    currentY += textRenderer_->FontHeight();
}

void
DisplayBase::drawLabel(PlanetProperties *planetProperties[])
{
    Options *options = Options::getInstance();
    if (!options->DrawLabel()) return;

    vector<string> labelLines;

    const body target = options->getTarget();
    const body origin = options->getOrigin();

    string lookAt;
    if (options->LabelString().empty())
    {
        if (options->TargetMode() != LOOKAT)
        {
            lookAt.assign("Looking at ");
            string viewTarget;
            string viewOrigin;

            if (options->Projection() == MULTIPLE)
            {
                viewTarget.assign(planetProperties[target]->Name());
                switch (options->OriginMode())
                {
                case ABOVE:
                    viewOrigin.assign(" from above");
                    break;
                case BELOW:
                    viewOrigin.assign(" from below");
                    break;
                case BODY:
                    if (options->OppositeSide())
                    {
                        viewTarget.assign(planetProperties[origin]->Name());
                        viewOrigin.assign(" from behind ");
                        viewOrigin += planetProperties[target]->Name();
                    }
                    else
                    {
                        viewOrigin.assign(" from ");
                        viewOrigin += planetProperties[origin]->Name();
                    }
                    break;
                case LBR:
                default:
                    break;
                }
            }
            else
            {
                viewTarget.assign(planetProperties[target]->Name());
            }

            lookAt += viewTarget;
            lookAt += viewOrigin;
        }
    }
    else
    {
        lookAt.assign(options->LabelString());

        for (unsigned int i = 0; i < lookAt.size() - 1; i++)
        {
            if (lookAt[i] == '%')
            {
                switch (lookAt[i+1])
                {
                case 't':
                    if (target < RANDOM_BODY)
                        lookAt.replace(i, 2, 
                                       planetProperties[target]->Name());
                    break;
                case 'o':
                    if (origin < RANDOM_BODY)
                        lookAt.replace(i, 2, 
                                       planetProperties[origin]->Name());
                    break;
                case '%':
                    lookAt.erase(i, 1);
                    break;
                }
            }
        }
    }

    time_t tv_sec = options->getTVSec();
    char timeString[128];
    if (tv_sec == (time_t) (-1))
    {
        int year, month, day, hour, min;
        double sec;
        double jd = options->getJulianDay();
        fromJulian(jd, year, month, day, hour, min, sec);
        snprintf(timeString, 128, 
                 "%4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d UTC",
                 year, month, day, 
                 hour, min, static_cast<int> (floor(sec)));
    }
    else
    {
        string tz_save("");
        if (options->DrawUTCLabel())
        {
            char *get_tz = getenv("TZ");
            if (get_tz != NULL)
            {
                tz_save = "TZ=";
                tz_save += get_tz;
            }
            putenv("TZ=UTC");
            tzset();
        }

        strftime(timeString, 128, options->DateFormat().c_str(),
                 localtime(&tv_sec));

        if (options->DrawUTCLabel())
        {
            if (tz_save.empty()) 
                removeFromEnvironment("TZ"); 
            else
                putenv((char *) tz_save.c_str()); 
            tzset();
        }       
    }

    if (!lookAt.empty()) labelLines.push_back(lookAt);
    labelLines.push_back(timeString);

    if (options->TargetMode() != LOOKAT)
    {
        char obsString[128];
        double obsLatDeg = options->Latitude() / deg_to_rad;
        double obsLonDeg = options->Longitude() / deg_to_rad;

        if (target == EARTH || target == MOON)
        {
            if (obsLonDeg > 180) obsLonDeg -= 360;
            snprintf(obsString, 128, "obs %4.1f %c %5.1f %c",
                     fabs(obsLatDeg), ((obsLatDeg < 0) ? 'S' : 'N'),
                     fabs(obsLonDeg), ((obsLonDeg < 0) ? 'W' : 'E'));
        }
        else
        {
            if (obsLonDeg < 0) obsLonDeg += 360;
            snprintf(obsString, 128,"obs %4.1f %c %5.1f",
                     fabs(obsLatDeg), ((obsLatDeg < 0) ? 'S' : 'N'),
                     obsLonDeg);
        }
        labelLines.push_back(obsString);

        if (target != SUN)
        {
            char sunString[128];
            double sunLatDeg = options->SunLat() / deg_to_rad;
            double sunLonDeg = options->SunLon() / deg_to_rad;
        
            if (target == EARTH || target == MOON)
            {
                if (sunLonDeg > 180) sunLonDeg -= 360;
                snprintf(sunString, 128, "sun %4.1f %c %5.1f %c",
                         fabs(sunLatDeg), ((sunLatDeg < 0) ? 'S' : 'N'),
                         fabs(sunLonDeg), ((sunLonDeg < 0) ? 'W' : 'E'));
            }
            else
            {
                if (sunLonDeg < 0) sunLonDeg += 360;
                snprintf(sunString, 128,"sun %4.1f %c %5.1f",
                         fabs(sunLatDeg), ((sunLatDeg < 0) ? 'S' : 'N'),
                         sunLonDeg);
            }
            labelLines.push_back(sunString);
        }
    }

    if (options->Projection() == MULTIPLE)
    {
        char fovCString[128];
        double fov = options->FieldOfView() / deg_to_rad;
        if (fov > 1)
            snprintf(fovCString, 128, "fov %.1f degrees", fov);
        else if (fov * 60 > 1)
        {
            fov *= 60;
            snprintf(fovCString, 128, "fov %.1f arc minutes", fov);
        }
        else if (fov * 3600 > 1)
        {
            fov *= 3600;
            snprintf(fovCString, 128, "fov %.1f arc seconds", fov);
        }
        else
        {
            fov *= 3600000;
            snprintf(fovCString, 128, "fov %.1f milliarc seconds", fov);
        }
        
        double oX, oY, oZ;
        options->getOrigin(oX, oY, oZ);

        double tX, tY, tZ;
        options->getTarget(tX, tY, tZ);

        const double deltX = tX - oX;
        const double deltY = tY - oY;
        const double deltZ = tZ - oZ;
        double targetDist = AU_to_km * sqrt(deltX*deltX 
                                            + deltY*deltY + deltZ*deltZ);
        
        char distString[128];
        if (targetDist < 1e6)
        {
            snprintf(distString, 128, "dist %.0f km", targetDist);
        }
        else if (targetDist < 1e9)
        {
            targetDist /= 1e6;
            snprintf(distString, 128, "dist %.1f million km", targetDist);
        }
        else
        {
            targetDist /= 1e9;
            snprintf(distString, 128, "dist %.1f billion km", targetDist);
        }

        labelLines.push_back(fovCString);
        labelLines.push_back(distString);

        if (options->TargetMode() != LOOKAT 
            && target != SUN)
        {
            char illumString[128];
            const double illumination = 50 * (ndot(tX, tY, tZ, 
                                                   deltX, deltY, deltZ) + 1);
            
            snprintf(illumString, 128, "illumination %.1f %%", illumination);
            labelLines.push_back(illumString);
        }
    }
    
    int labelX = options->LabelX();
    int labelY = options->LabelY() + textRenderer_->FontHeight()/2;

    if (options->LabelMask() & YNegative)
    {
        labelY += (height_ - labelLines.size() * textRenderer_->FontHeight());
    }

    for (unsigned int i = 0; i < labelLines.size(); i++)
    {
        if (!labelLines[i].empty())
            drawLabelLine(labelX, labelY, labelLines[i]);
    }
}

// Set the pixel value to { value, value, value }
void
DisplayBase::setPixel(const int x, const int y, const unsigned int value)
{
    if (x < 0 || x >= width_ || y < 0 || y >= height_) return;

    unsigned char *background = rgb_data + 3*(y*width_ + x);
    memset(background, value, 3);
    if (alpha != NULL) alpha[y*width_ + x] =  255;
}

// Given floating point pixel values, spread the pixel around its
// neighbors
void
DisplayBase::setPixel(const double X, const double Y, 
                      const unsigned char color[3])
{
    if (X < 0 || X >= width_ || Y < 0 || Y >= height_) return;

    const int x0 = static_cast<int> (floor(X));
    const int y0 = static_cast<int> (floor(Y));

    int ipos[4];
    ipos[0] = y0 * width_ + x0;
    ipos[1] = ipos[0] + 1;
    ipos[2] = ipos[0] + width_;
    ipos[3] = ipos[2] + 1;
    
    const double t = X - x0;
    const double u = 1 - (Y - y0);
    
    double weight[4];
    getWeights(t, u, weight);

    setPixel(x0, y0, color, weight[0]);
    setPixel(x0+1, y0, color, weight[1]);
    setPixel(x0, y0+1, color, weight[2]);
    setPixel(x0+1, y0+1, color, weight[3]);
}

void
DisplayBase::setPixel(const int x, const int y, const unsigned char pixel[3])
{
    setPixel(x, y, pixel, 1.0);
}

void
DisplayBase::setPixel(const int x, const int y, const unsigned char p[3],
                      const double opacity)
{
    if (x < 0 || x >= width_ || y < 0 || y >= height_) return;

    unsigned char *background = rgb_data + 3*(y*width_ + x);
    unsigned char pixel[3];
    memcpy(pixel, p, 3);
    if (opacity < 1)
    {
        for (int i = 0; i < 3; i++)
            pixel[i] = (unsigned char) (opacity * p[i] 
                                        + (1 - opacity) * background[i]);
        if (alpha != NULL) 
            alpha[y*width_ + x] = (unsigned char) (opacity * 255);
    }
    else
    {
        if (alpha != NULL) alpha[y*width_ + x] = 255;
    }

    memcpy(background, pixel, 3);
}

void
DisplayBase::getPixel(const int x, const int y, unsigned char pixel[3]) const
{
    if (x < 0 || x >= width_ || y < 0 || y >= height_) return;

    memcpy(pixel, rgb_data + 3*(y*width_ + x), 3);
}

void
DisplayBase::SetBackground(const int width, const int height, 
                           unsigned char *rgb)
{
    Options *options = Options::getInstance();
    string backgroundFile(options->Background());
    if (!backgroundFile.empty())
    {
        Image *image = new Image;
        bool foundFile = findFile(backgroundFile, "images");
        if (foundFile) 
            foundFile = image->Read(backgroundFile.c_str());
        
        if (foundFile)
        {
            if ((image->Width() != width)
                || (image->Height() != height))
            {
                ostringstream errStr;
                errStr << "For better performance, "
                       << "background image should "
                       << "be the same size as the output image\n";
                xpWarn(errStr.str(), __FILE__, __LINE__);
                image->Resize(width, height);
            }
            memcpy(rgb, image->getRGBData(), 3 * width * height);
        }
        delete image;
    }
    else
    {
        if (options->Projection() != MULTIPLE)
        {
            // add random stars
            int numStars = static_cast<int> (width * height 
                                             * options->StarFreq());
            for (int i = 0; i < numStars; i++)
            {
                int j = random() % width;
                int k = random() % height;
                int brightness = random() % 256;
                memset(rgb + 3 * (k * width + j), brightness, 3);
            }
        }
    }
}

void
DisplayBase::allocateRGBData()
{
    area_ = width_ * height_;
    rgb_data = new unsigned char [3 * area_];
    memset(rgb_data, 0, 3 * area_);

    alpha = NULL;
    Options *options = Options::getInstance();

    if (options->TransPNG())
    {
        alpha = new unsigned char [area_];
        memset(alpha, 0, area_);
    }

    // If a background image is specified along with -geometry and
    // we're drawing to the root window, it will only be overlaid on
    // the true root window. The sub-image won't have the background
    // image.
    if (options->DisplayMode() != ROOT
        || !options->GeometrySelected())
        SetBackground(width_, height_, rgb_data);
}

string
DisplayBase::TmpDir()
{
    Options *options = Options::getInstance();

    string returnstring = options->TmpDir();
    if (returnstring.empty())
    {
        char *tmpdir = getenv("TMPDIR");
        if (tmpdir == NULL) 
            returnstring.assign("/tmp");
        else
            returnstring.assign(tmpdir);
    }
    return(returnstring);
}

// If -geometry is specified, overlay the image on the root window.
void
DisplayBase::PlaceImageOnRoot()
{
    Options *options = Options::getInstance();

    const int area = fullWidth_ * fullHeight_;
    unsigned char *tmp = new unsigned char [ 3 * area ];
    memset(tmp, 0, 3 * area);

    SetBackground(fullWidth_, fullHeight_, tmp);
    
    int x = options->getWindowX();
    int y = options->getWindowY();
    if (options->GeometryMask() & XNegative) 
        x += (fullWidth_ - width_);
    if (options->GeometryMask() & YNegative) 
        y += (fullHeight_ - height_);
    
    const int xmin = (x < 0) ? 0 : x;
    const int ymin = (y < 0) ? 0 : y;
    const int xmax = (x + width_ > fullWidth_) ? fullWidth_ : x + width_;
    const int ymax = (y + height_ > fullHeight_) ? fullHeight_ : y + height_;
    
    for (int trueY = ymin; trueY < ymax; trueY++)
    {
        unsigned char *trueP = tmp + 3 * trueY * fullWidth_;
        const int windowY = trueY - ymin;
        unsigned char *windowP = rgb_data + 3 * windowY * width_;
        for (int trueX = xmin; trueX < xmax; trueX++)
        {
            const int windowX = trueX - xmin;
            memcpy(trueP + 3 * trueX, windowP + 3 * windowX, 3);
        }
    }
    
    delete [] rgb_data;
    rgb_data = tmp;
}
