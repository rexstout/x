#ifndef SYMBOL_H
#define SYMBOL_H

#include "Annotation.h"

class Symbol : public Annotation
{
 public:
    Symbol(const unsigned char color[3], 
           const int x, const int y, const int r);

    virtual ~Symbol();

    void Outline(const bool b) { outlined_ = b; };

    virtual void Shift(const int x) { x_ += x; };
    virtual void Draw(DisplayBase *display);

 private:
    int x_;
    const int y_, r_;
    bool outlined_;

    void DrawCircle(DisplayBase *display, const int r, 
                    const unsigned char color[3]);
};

#endif
