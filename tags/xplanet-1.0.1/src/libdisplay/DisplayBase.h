#ifndef DisplayBase_h
#define DisplayBase_h

#include <string>
#include <vector>

#include "config.h"

#ifdef HAVE_LIBFREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#endif

class PlanetProperties;

class DisplayBase
{
public:
    DisplayBase(const int tr);
    virtual ~DisplayBase();

    int Width() const { return(width_); };
    int Height() const { return(height_); };

    void ForegroundColor(const unsigned char color[3]) 
	{ memcpy(foregroundColor, color, 3); };

    void setPixel(const double X, const double Y,
		  const unsigned char color[3]);
    void setPixel(const int x, const int y, const unsigned int value);
    void setPixel(const int x, const int y, const unsigned char pixel[3]);
    void setPixel(const int x, const int y, const unsigned char pixel[3],
		  const double opacity);
    void getPixel(const int x, const int y, unsigned char pixel[3]) const;

    virtual void renderImage(PlanetProperties *planetProperties[]) = 0;

    const std::string & Font() const { return(font_); };
    int FontSize() const { return(fontSize_); };

    void Font(const std::string &fontname);
    void FontSize(const int size);

    void setText(const std::string &text);
    void DrawOutlinedText(const int x, int y, const std::string &text, 
			  const unsigned char color[3]);
    void FreeText();
    void getTextBox(int &textWidth, int &textHeight);

    virtual std::string TmpDir();

protected:
    const int times_run;

    int width_, height_;       // pixel dimensions of the display
    int area_;
    unsigned char *rgb_data;
    unsigned char *alpha;

    void allocateRGBData();
    void drawLabel(PlanetProperties *planetProperties[]);
    void drawLabelLine(int &currentX, int &currentY, const std::string &text);

private:
    unsigned char foregroundColor[3];

    bool CheckUnicode(const unsigned long unicode, 
		      const vector<unsigned char> &text);

    unsigned long UTF8ToUnicode(const std::vector<unsigned char> &text);
    void drawText(const int x, const int y, const unsigned char color[3]);

    void drawUTF8test();
#ifdef HAVE_LIBFREETYPE
    FT_Library library_;
    FT_Face face_;

    FT_Glyph *glyphs_;
    FT_Vector *pos;
    FT_UInt numGlyphs_;
#endif

    std::string font_;
    int fontSize_;
};
#endif
