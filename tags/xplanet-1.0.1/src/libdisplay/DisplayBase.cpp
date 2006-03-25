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

DisplayBase::DisplayBase(const int tr) : times_run(tr)
{
#ifdef HAVE_LIBFREETYPE 
    glyphs_ = NULL;
    pos = NULL;
    numGlyphs_ = 0;

    const int error = FT_Init_FreeType(&library_);
    if (error)
        xpExit("Can't initialize freetype library\n", __FILE__, __LINE__);

    Options *options = Options::getInstance();
    fontSize_ = options->FontSize();

    Font(options->Font());
#endif
}

DisplayBase::~DisplayBase()
{
    delete [] rgb_data;
    delete [] alpha;

#ifdef HAVE_LIBFREETYPE
    FT_Done_Face(face_);
    FT_Done_FreeType(library_);

    delete [] glyphs_;
    delete [] pos;
#endif
}

void
DisplayBase::Font(const string &fontname)
{
#ifdef HAVE_LIBFREETYPE
    font_.assign(fontname);

    if (!findFile(font_, "fonts"))
    {
        ostringstream errStr;
        errStr << "Can't open font file " << font_ << endl;
        xpWarn(errStr.str(), __FILE__, __LINE__);
        font_ = defaultFont;
        if (!findFile(font_, "fonts"))
        {
            errStr.str("");
            errStr << "Can't open default font file " << font_ << endl;
            xpExit(errStr.str(), __FILE__, __LINE__);
        }
    }

    int error = FT_New_Face(library_, font_.c_str(), 0, &face_);
    if (error)
    {
        ostringstream errStr;
        errStr << "Can't load font " << font_ << endl;
        xpExit(errStr.str(), __FILE__, __LINE__);
    }

    error = FT_Select_Charmap(face_, ft_encoding_unicode);
    if (error)
    {
        ostringstream errStr;
        errStr << "No unicode map in font " << font_ << endl;
        xpExit(errStr.str(), __FILE__, __LINE__);
    }

    FontSize(fontSize_);
#endif
}

void
DisplayBase::FontSize(const int size)
{
#ifdef HAVE_LIBFREETYPE
    fontSize_ = size;
    int error = FT_Set_Pixel_Sizes(face_, 0, fontSize_);
    if (error) 
    {
        ostringstream errStr;
        errStr << "Can't set pixel size to " << fontSize_ << endl;
        xpWarn(errStr.str(), __FILE__, __LINE__);
        fontSize_ = 12;
        error = FT_Set_Pixel_Sizes(face_, 0, fontSize_);
    }
    if (error)
    {
	ostringstream errStr;
	errStr << "Can't set pixel size to " << fontSize_ << "\n";
        xpExit(errStr.str(), __FILE__, __LINE__);
    }
#endif
}

void
DisplayBase::getTextBox(int &textWidth, int &textHeight)
{
#ifdef HAVE_LIBFREETYPE
    FT_BBox  bbox;
    
    // initialise string bbox to "empty" values
    bbox.xMin = bbox.yMin =  32000;
    bbox.xMax = bbox.yMax = -32000;
    
    // for each glyph image, compute its bounding box, translate it,
    // and grow the string bbox
    for (unsigned int i = 0; i < numGlyphs_; i++)
    {
        FT_BBox   glyph_bbox;

        FT_Glyph_Get_CBox( glyphs_[i], ft_glyph_bbox_pixels, &glyph_bbox );
        
        glyph_bbox.xMin += pos[i].x;
        glyph_bbox.xMax += pos[i].x;
        glyph_bbox.yMin += pos[i].y;
        glyph_bbox.yMax += pos[i].y;
        
        if (glyph_bbox.xMin < bbox.xMin)
            bbox.xMin = glyph_bbox.xMin;
        
        if (glyph_bbox.yMin < bbox.yMin)
            bbox.yMin = glyph_bbox.yMin;
        
        if (glyph_bbox.xMax > bbox.xMax)
            bbox.xMax = glyph_bbox.xMax;
        
        if (glyph_bbox.yMax > bbox.yMax)
            bbox.yMax = glyph_bbox.yMax;
    }
    
    // check that we really grew the string bbox
    if ( bbox.xMin > bbox.xMax )
    {
        bbox.xMin = 0;
        bbox.yMin = 0;
        bbox.xMax = 0;
        bbox.yMax = 0;    
    }

    textWidth = bbox.xMax - bbox.xMin;
    textHeight = bbox.yMax - bbox.yMin;
#endif
}

void
DisplayBase::drawUTF8test()
{
#ifdef HAVE_LIBFREETYPE
    Options *options = Options::getInstance();
    ifstream inFile("UTF-8-test.txt");
    char line[256];
    int x = 0;
    int y = 0;
    while(inFile.getline (line, 256, '\n') != NULL)
    {
        DrawOutlinedText(x, y, line, options->Color());
        y += face_->size->metrics.y_ppem;
        if (y > height_) 
        {
            y = 0;
            x += width_/3;
        }
    }
#endif
}

bool
DisplayBase::CheckUnicode(const unsigned long unicode, 
                          const vector<unsigned char> &text)
{
    int curPos;
    int numWords;
    if (unicode < 0x00000080)
    {
        curPos = 6;
        numWords = 1;
    }
    else if (unicode < 0x00000800)
    {
        curPos = 10;
        numWords = 2;
    }
    else if (unicode < 0x00010000)
    {
        curPos = 15;
        numWords = 3;
    }
    else if (unicode < 0x00200000)
    {
        curPos = 20;
        numWords = 4;
    }
    else if (unicode < 0x04000000)
    {
        curPos = 25;
        numWords = 5;
    }
    else if (unicode < 0x80000000)
    {
        curPos = 30;
        numWords = 6;
    }
    else
    {
        xpWarn("Bad unicode value\n", __FILE__, __LINE__);
        return(false);
    }
    
    // Construct the smallest UTF-8 encoding for this unicode value.
    bitset<32> uBitset(unicode);
    string utf8Val;
    if (numWords == 1) 
    {
        utf8Val += "0";
        for (int i = 0; i < 7; i++)
            utf8Val += (uBitset.test(curPos--) ? "1" : "0");
    }
    else
    {
        for (int i = 0; i < numWords; i++)
            utf8Val += "1";
        utf8Val += "0";
        for (int i = 0; i < 7 - numWords; i++)
            utf8Val += (uBitset.test(curPos--) ? "1" : "0");
        for (int j = 0; j < numWords - 1; j++)
        {
            utf8Val += "10";
            for (int i = 0; i < 6; i++)
                utf8Val += (uBitset.test(curPos--) ? "1" : "0");
        }
    }

    // Check that the input array is the "correct" array for
    // generating the derived unicode value.
    vector<unsigned char> utf8Vec;
    for (unsigned int i = 0; i < utf8Val.size(); i += 8)
    {
        string thisByte(utf8Val.substr(i, i+8));
        utf8Vec.push_back(bitset<8>(thisByte).to_ulong() & 0xff);
    }

    bool goodValue = (text.size() == utf8Vec.size());
    if (goodValue)
    {
        for (unsigned int i = 0; i < utf8Vec.size(); i++)
        {
            if (text[i] != utf8Vec[i])
            {
                goodValue = false;
                break;
            }
        }
    }

    return(goodValue);
}

// given an array in UTF-8, find the unicode value
unsigned long
DisplayBase::UTF8ToUnicode(const vector<unsigned char> &text)
{
    unsigned long returnVal = 0xfffd;  // Unknown character 

    if (text.size() == 1) 
    {
        if (text[0] < 0x80)
        {
            returnVal = static_cast<unsigned long> (text[0] & 0x7f);
        }
        else
        {
            ostringstream errStr;
            errStr << "Multibyte UTF-8 code in single byte encoding:\n";
            for (unsigned int i = 0; i < text.size(); i++)
                errStr << hex << static_cast<int> (text[i]) << dec;
            errStr << endl;
            xpWarn(errStr.str(), __FILE__, __LINE__);
        }
        return(returnVal);
    }
    else if (text.size() > 6)
    {
        ostringstream errStr;
        errStr << "Too many bytes in UTF-8 sequence:\n";
        for (unsigned int i = 0; i < text.size(); i++)
            errStr << "(" << hex << static_cast<int> (text[i]) << dec << ")";
        errStr << endl;
        xpWarn(errStr.str(), __FILE__, __LINE__);
        return(returnVal);
    }

    bool goodChar = (text[0] >= 0xc0 && text[0] <= 0xfd);
    if (!goodChar)
    {
        ostringstream errStr;
        errStr << "Invalid leading byte in UTF-8 sequence:\n";
        for (unsigned int i = 0; i < text.size(); i++)
            errStr << "(" << hex << static_cast<int> (text[i]) << dec << ")";
        errStr << endl;
        xpWarn(errStr.str(), __FILE__, __LINE__);
        return(returnVal);
    }

    for (int i = 1; i < text.size(); i++)
    {
        goodChar = (text[i] >= 0x80 && text[i] <= 0xbf);
        if (!goodChar)
        {
            ostringstream errStr;
            errStr << "Invalid continuation byte in UTF-8 sequence:\n";
            for (unsigned int i = 0; i < text.size(); i++)
                errStr << hex << "(" << hex
		       << static_cast<int> (text[i]) << dec << ")";
            errStr << endl;
            xpWarn(errStr.str(), __FILE__, __LINE__);
            return(returnVal);
        }
    }

    bitset<8> firstByte(static_cast<unsigned char>(text[0]));

    int numBytes = 0;
    while(firstByte.test(7 - numBytes)) numBytes++;

    string binValue;
    for (int i = 6 - numBytes; i >= 0; i--)
        binValue += (firstByte.test(i) ? "1" : "0");
            
    for (int j = 1; j < numBytes; j++)
    {
        bitset<8> thisByte(static_cast<unsigned char>(text[j]));
        for (int i = 5; i >= 0; i--)
            binValue += (thisByte.test(i) ? "1" : "0");
    }

    returnVal = bitset<32>(binValue).to_ulong();

    if ((returnVal >= 0xd800 && returnVal <= 0xdfff)     // U+D800 to U+DFFF (UTF-16 surrogates)
        || (returnVal == 0xfffe || returnVal == 0xffff)    // U+FFFE and U+FFFF are just plain illegal
        || (!CheckUnicode(returnVal, text)))
    { 
        ostringstream errStr;
        errStr << "Malformed UTF-8 sequence:\n";
        for (unsigned int i = 0; i < text.size(); i++)
            errStr << "(" << hex << static_cast<int> (text[i]) << dec << ")";
        errStr << endl;
        xpWarn(errStr.str(), __FILE__, __LINE__);
        returnVal = 0xfffd;
    }
    return(returnVal);
}

// Remember to call FreeText() when done with the text operation
void
DisplayBase::setText(const string &text)
{
#ifdef HAVE_LIBFREETYPE
    unsigned int numChars = 0;
    
    vector<unsigned long> unicodeText;
    vector<unsigned char> utf8Text;
    for (unsigned int i = 0; i < text.size(); i++)
    {
        unsigned char thisByte = (text[i] & 0xff);
        if (thisByte < 0x80 || (thisByte >= 0xc0 && thisByte <= 0xfd))
        {
            // This is either an ASCII character or the first byte of
            // a multibyte sequence
            if (!utf8Text.empty()) 
            {
                numChars++;
                unicodeText.push_back(UTF8ToUnicode(utf8Text));
            }
            utf8Text.clear();
        }
        utf8Text.push_back(thisByte);
    }
    if (!utf8Text.empty()) 
    {   numChars++;
    unicodeText.push_back(UTF8ToUnicode(utf8Text));
    }
  
    int pen_x = 0;   /* start at (0,0) !! */
    int pen_y = 0;
    
    FT_Bool use_kerning = FT_HAS_KERNING(face_);
    FT_UInt previous = 0;
    
    delete [] glyphs_;
    delete [] pos;

    glyphs_ = new FT_Glyph[text.size()];
    pos = new FT_Vector[text.size()];
    numGlyphs_ = 0;

    for (unsigned int n = 0; n < numChars; n++ )
    {
        // convert character code to glyph index
        FT_UInt glyph_index = FT_Get_Char_Index( face_, unicodeText[n] );
        
        // retrieve kerning distance and move pen position
        if ( use_kerning && previous && glyph_index )
        {
            FT_Vector delta;
            FT_Get_Kerning( face_, previous, glyph_index,
                            ft_kerning_default, &delta );
            pen_x += delta.x >> 6;
        }

        pos[numGlyphs_].x = pen_x;
        pos[numGlyphs_].y = pen_y;

        // load glyph image into the slot. DO NOT RENDER IT !!
        int error = FT_Load_Glyph( face_, glyph_index, FT_LOAD_DEFAULT );
        if (error) continue;  // ignore errors, jump to next glyph

        // extract glyph image and store it in our table
        error = FT_Get_Glyph( face_->glyph, &glyphs_[numGlyphs_] );
        if (error) continue;  // ignore errors, jump to next glyph

        // increment pen position
        pen_x += face_->glyph->advance.x >> 6;

        // record current glyph index
        previous = glyph_index;

        numGlyphs_++;
    }
#endif
}

// x and y are the lower left coordinates of the string
void 
DisplayBase::drawText(const int x, const int y, const unsigned char color[3])
{
#ifdef HAVE_LIBFREETYPE
    for (unsigned int i = 0; i < numGlyphs_; i++)
    {
        FT_Glyph image;
        int error = FT_Glyph_Copy(glyphs_[i], &image);

        FT_Vector pen;
        pen.x = x + pos[i].x;
        pen.y = y + pos[i].y;
        
        error = FT_Glyph_To_Bitmap( &image, ft_render_mode_normal,
                                    &pen, 1 );

        if (!error) 
        {
            FT_BitmapGlyph bit = (FT_BitmapGlyph) image;
            FT_Bitmap bitmap = bit->bitmap;
            for (int j = 0; j < bitmap.rows; j++)
            {
                int istart = j * bitmap.width;
                for (int k = 0; k < bitmap.width; k++)
                {
                    if (bitmap.buffer[istart + k])
                    {
                        const double opacity = bitmap.buffer[istart + k]/255.0;
                        setPixel(pen.x + bit->left + k, 
                                 pen.y - bit->top + j, 
                                 color, opacity);
                    }
                }
            }
            FT_Done_Glyph(image);
        }
    }
#endif
}

void
DisplayBase::FreeText()
{
#ifdef HAVE_LIBFREETYPE
    for (unsigned int i = 0; i < numGlyphs_; i++)
        FT_Done_Glyph(glyphs_[i]);
#endif
}

// x and y are the center left coordinates of the string
void 
DisplayBase::DrawOutlinedText(const int x, int y, const string &text, 
                              const unsigned char color[3])
{
#ifdef HAVE_LIBFREETYPE
    setText(text);

    int textWidth, textHeight;
    getTextBox(textWidth, textHeight);

    y += textHeight/2;

    unsigned char black[3] = { 0, 0, 0 };
    drawText(x+1, y, black);
    drawText(x-1, y, black);
    drawText(x, y+1, black);
    drawText(x, y-1, black);

    drawText(x, y, color);

    FreeText();
#endif
}

void
DisplayBase::drawLabelLine(int &currentX, int &currentY, const string &text)
{
#ifdef HAVE_LIBFREETYPE
    Options *options = Options::getInstance();

    if (options->LabelMask() & XNegative) 
    {
        setText(text);
        
        int textWidth, textHeight;
        getTextBox(textWidth, textHeight);

        FreeText();

        currentX = (options->LabelX() + width_ - textWidth);
    }
    currentY += face_->size->metrics.y_ppem;
    DrawOutlinedText(currentX, currentY, text, options->Color());
#endif
}

void
DisplayBase::drawLabel(PlanetProperties *planetProperties[])
{
#ifdef HAVE_LIBFREETYPE
    Options *options = Options::getInstance();
    if (!options->DrawLabel()) return;

    vector<string> labelLines;

    const body target = options->getTarget();

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
		viewTarget.assign(planetProperties[options->getOrigin()]->Name());
		viewOrigin.assign(" from behind ");
		viewOrigin += planetProperties[target]->Name();
	    }
	    else
	    {
		viewOrigin.assign(" from ");
		viewOrigin += planetProperties[options->getOrigin()]->Name();
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

    string lookAt("Looking at ");
    lookAt += viewTarget;
    lookAt += viewOrigin;

    time_t tv_sec = options->getTVSec();
    char timeString[128];
    if (tv_sec == (time_t) (-1))
    {
        int year, month, day, hour, min;
        double sec;
        double jd = options->getJulianDay();
        fromJulian(jd, year, month, day, hour, min, sec);
        snprintf(timeString, 128, 
                 "%4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d GMT",
                 year, month, day, 
                 hour, min, (int) floor(sec));
    }
    else
    {
        string tz_save("");
        if (options->DrawGMTLabel())
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

        if (options->DrawGMTLabel())
        {
            if (tz_save.empty()) 
                removeFromEnvironment("TZ"); 
            else
                putenv((char *) tz_save.c_str()); 
            tzset();
        }       
    }

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

    labelLines.push_back(lookAt);
    labelLines.push_back(timeString);
    labelLines.push_back(obsString);

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
        if (target != SUN)
        {
            char illumString[128];
            const double illumination = 50 * (ndot(tX, tY, tZ, 
                                                   deltX, deltY, deltZ) + 1);
            
            snprintf(illumString, 128, "illumination %.1f %%", illumination);
            labelLines.push_back(illumString);
        }
    }
    
    int labelX = options->LabelX();
    int labelY = options->LabelY();
    if (options->LabelMask() & XNegative) labelX += width_;

    if (options->LabelMask() & YNegative)
    {
        const int dY = face_->size->metrics.y_ppem;
        labelY += (height_ - (1 + labelLines.size()) * dY);
    }

    for (unsigned int i = 0; i < labelLines.size(); i++)
        drawLabelLine(labelX, labelY, labelLines[i]);
#endif
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

    int ipos[4];
    ipos[0] = ((int) floor(Y)) * width_ + ((int) floor(X));
    ipos[1] = ipos[0] + 1;
    ipos[2] = ipos[0] + width_;
    ipos[3] = ipos[2] + 1;
    
    const double t = X - floor(X);
    const double u = 1 - (Y - floor(Y));
    
    double weight[4];
    getWeights(t, u, weight);

    for (int i = 0; i < 4; i++)
    {
        if (ipos[i] >= area_) ipos[i] = ipos[0];
        const int istart = 3 * ipos[i];
        for (int j = 0; j < 3; j++)
        {
            const double sum = weight[i] * color[j];
            if (sum + rgb_data[istart + j] > 255)
                rgb_data[istart + j] = 255;
            else
                rgb_data[istart + j] += (unsigned char) sum;
        }
        if (alpha != NULL)
        {
            const double sum = (color[0] + color[1] + color[2]) / 3;
            if (sum + alpha[ipos[i]] > 255)
                alpha[ipos[i]] = 255;
            else
                alpha[ipos[i]] += (unsigned char) sum;
        }
    }
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

    string backgroundFile(options->Background());
    if (!backgroundFile.empty())
    {
	Image *image = new Image;
	bool foundFile = findFile(backgroundFile, "images");
	if (foundFile) 
	    foundFile = image->Read(backgroundFile.c_str());
	
	if (foundFile)
	{
	    if ((image->Width() != width_)
		|| (image->Height() != height_))
	    {
		ostringstream errStr;
		errStr << "Warning: For better performance, "
		       << "background image should "
		       << "be the same size as the output image\n";
		xpWarn(errStr.str(), __FILE__, __LINE__);
		image->Resize(width_, height_);
	    }
	    memcpy(rgb_data, image->getRGBData(), 3 * width_ * height_);
	}
	delete image;
    }
    else
    {
	if (options->Projection() != MULTIPLE)
	{
            // add random stars
            int numStars = static_cast<int> (width_ * height_ * options->StarFreq());
            for (int i = 0; i < numStars; i++)
            {
                int j = random() % width_;
                int k = random() % height_;
                int brightness = random() % 256;
                memset(rgb_data + 3 * (k * width_ + j), brightness, 3);
            }
        }
    }
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
