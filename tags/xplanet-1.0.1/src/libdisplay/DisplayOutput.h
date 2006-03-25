#ifndef DISPLAYOUTPUT_H
#define DISPLAYOUTPUT_H

#include "DisplayBase.h"

class DisplayOutput : public DisplayBase
{
 public:
    DisplayOutput(const int tr);
    virtual ~DisplayOutput();

    void renderImage(PlanetProperties *planetProperties[]);

 private:
    int quality_;

    const char * constructOutputFilename();
};

#endif
