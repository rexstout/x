#include <cmath>
#include <map>
using namespace std;

#include "body.h"
#include "Options.h"
#include "PlanetProperties.h"
#include "View.h"

#include "libannotate/LineSegment.h"
#include "libmultiple/libmultiple.h"
#include "libplanet/Planet.h"

static void
addArc(const double startTime, const double stopTime, 
       const int numTimes, const unsigned char color[3],
       const View *view, 
       const int width, const int height, 
       const double Prx, const double Pry, const double Prz,
       Planet *p, multimap<double, Annotation *> &annotationMap)
{
    Options *options = Options::getInstance();

    const body b = p->Index();
    const bool isTarget = (b == options->getTarget());

    const double delTime = (stopTime - startTime) / numTimes;
    bool firstTime = true;
    double prevX = 0, prevY = 0, prevZ = 0;
    for (int i = 0; i <= numTimes; i++)
    {
        const double jd = startTime + i * delTime;

        double X, Y, Z;
        Planet planet(jd, b);
        planet.calcHeliocentricEquatorial(false);
        planet.getPosition(X, Y, Z);
            
        view->XYZToPixel(X + Prx, Y + Pry, Z + Prz,
                         X, Y, Z);
        X += options->CenterX();
        Y += options->CenterY();

        if (!firstTime)
        {
            double nearX, nearY, nearZ;
            double farX, farY, farZ;

            if (prevZ > Z)
            {
                nearX = X; 
                nearY = Y; 
                nearZ = Z;
                farX = prevX; 
                farY = prevY;
                farZ = prevZ;
            }
            else
            {
                nearX = prevX; 
                nearY = prevY;
                nearZ = prevZ;
                farX = X; 
                farY = Y; 
                farZ = Z;
            }

            const bool skip = ((prevX < 0 && X < 0)
                               || (prevY < 0 && Y < 0)
                               || (prevX >= width && X >= width)
                               || (prevY >= height && Y >= height)
                               || (fabs(X) > width*100)
                               || (fabs(Y) > height*100)
                               || (fabs(prevX) > width*100)
                               || (fabs(prevY) > height*100)
                               || (prevZ < 0 && Z < 0));

            if (!skip || (i == 1 && isTarget)) 
            {
                LineSegment *ls = new LineSegment(color, 
                                                  farX, farY,
                                                  nearX, nearY);
                annotationMap.insert(pair<const double, Annotation*>(farZ, ls));
            }
        }

        prevX = X;
        prevY = Y;
        prevZ = Z;

        firstTime = false;
    }
}

void
addOrbits(const double jd0, const View *view, 
          const int width, const int height, 
          Planet *p,  PlanetProperties *currentProperties, 
          multimap<double, Annotation *> &annotationMap)
{
    const double period = p->Period();
    if (period == 0) return;

    const double startOrbit = currentProperties->StartOrbit();
    const double stopOrbit = currentProperties->StopOrbit();
    const double delOrbit = currentProperties->DelOrbit();
    const unsigned char *color = currentProperties->OrbitColor();

    double Prx=0, Pry=0, Prz=0;
    if (p->Primary() != SUN)
    {
        Planet primary(jd0, p->Primary());
        primary.calcHeliocentricEquatorial();
        primary.getPosition(Prx, Pry, Prz);
    }

    const double startTime = jd0 + startOrbit * period;
    const double stopTime = jd0 + stopOrbit * period;

    const int numTimes = (int) (180/delOrbit + 0.5);

    addArc(startTime, jd0, numTimes, color, view, width, height, 
           Prx, Pry, Prz, p, annotationMap);
    addArc(jd0, stopTime, numTimes, color, view, width, height, 
           Prx, Pry, Prz, p, annotationMap);
}
