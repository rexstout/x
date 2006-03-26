#include <cmath>
using namespace std;

#include "LineSegment.h"

#include "libdisplay/libdisplay.h"

LineSegment::LineSegment(const unsigned char color[3], 
                         const double X0, const double Y0,
                         const double X1, const double Y1)
    : Annotation(color), X0_(X0), Y0_(Y0), X1_(X1), Y1_(Y1)
{
}

LineSegment::~LineSegment()
{
}

void
LineSegment::Draw(DisplayBase *display)
{
    const double width = display->Width();
    const double height = display->Height();

    if ((X0_ < 0 && X1_ < 0)
        || (X0_ >= width && X1_ >= width)
        || (Y0_ < 0 && Y1_ < 0)
        || (Y0_ >= height && Y1_ >= height)) return;

    if (X0_ == X1_)
    {
        for (double y = Y0_; y < Y1_; y++)
            display->setPixel(X0_, y, color_);
        return;
    }

    const double slope = (Y1_ - Y0_) / (X1_ - X0_);

    if (fabs(slope) < 1)
    {
        double y = Y0_;
        double x0 = X0_;
        double x1 = X1_;
        if (X0_ > X1_)
        {
            y = Y1_;
            x0 = X1_;
            x1 = X0_;
        }
        if (x0 < 0) 
        {
            y -= slope * x0;
            x0 = 0;
        }
        if (x1 >= width) x1 = width - 1;
        for (double x = x0; x < x1; x++)
        {
            display->setPixel(x, y, color_);
            y += slope;
        }
    }
    else
    {
        double x = X0_;
        double y0 = Y0_;
        double y1 = Y1_;
        if (Y0_ > Y1_)
        {
            x = X1_;
            y0 = Y1_;
            y1 = Y0_;
        }
        if (y0 < 0) 
        {
            x -= y0/slope;
            y0 = 0;
        }
        if (y1 >= height) y1 = height - 1;
        for (double y = y0; y < y1; y++)
        {
            display->setPixel(x, y, color_);
            x += (1/slope);
        }
    }
}
