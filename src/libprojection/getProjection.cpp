#include <cctype>
#include <cstring>
using namespace std;

#include "keywords.h"
#include "xpUtil.h"

#include "ProjectionBase.h"
#include "ProjectionAncient.h"
#include "ProjectionAzimuthal.h"
#include "ProjectionHemisphere.h"
#include "ProjectionLambert.h"
#include "ProjectionMercator.h"
#include "ProjectionMollweide.h"
#include "ProjectionOrthographic.h"
#include "ProjectionPeters.h"
#include "ProjectionRectangular.h"

int 
getProjectionType(char *proj_string)
{
    int projection;

    char *lowercase = proj_string;
    char *ptr = proj_string;
    while (*ptr != '\0') *ptr++ = tolower(*proj_string++);
    if (strncmp(lowercase, "ancient", 2) == 0)
        projection = ANCIENT;
    else if (strncmp(lowercase, "azimuthal", 2) == 0)
        projection = AZIMUTHAL;
    else if (strncmp(lowercase, "hemisphere", 1) == 0)
        projection = HEMISPHERE;
    else if (strncmp(lowercase, "lambert", 1) == 0)
        projection = LAMBERT;
    else if (strncmp(lowercase, "mercator", 2) == 0)
        projection = MERCATOR;
    else if (strncmp(lowercase, "mollweide", 2) == 0)
        projection = MOLLWEIDE;
    else if (strncmp(lowercase, "orthographic", 1) == 0)
        projection = ORTHOGRAPHIC;
    else if (strncmp(lowercase, "peters", 1) == 0)
        projection = PETERS;
    else if (strncmp(lowercase, "rectangular", 1) == 0)
        projection = RECTANGULAR;
    else 
    {
        xpWarn("Unknown projection, using rectangular\n", 
	       __FILE__, __LINE__);
        projection = RECTANGULAR;
    }
    return(projection);
}

ProjectionBase *
getProjection(const int projection, const int flipped,
	      const int width, const int height)
{
    ProjectionBase *thisProjection = NULL;
    switch (projection)
    {
    case ANCIENT:
        thisProjection = new ProjectionAncient(flipped, width, height);
        break;
    case AZIMUTHAL:
        thisProjection = new ProjectionAzimuthal(flipped, width, height);
        break;
    case HEMISPHERE:
        thisProjection = new ProjectionHemisphere(flipped, width, height);
        break;
    case LAMBERT:
        thisProjection = new ProjectionLambert(flipped, width, height);
        break;
    case MERCATOR:
        thisProjection = new ProjectionMercator(flipped, width, height);
        break;
    case MOLLWEIDE:
        thisProjection = new ProjectionMollweide(flipped, width, height);
        break;
    case ORTHOGRAPHIC:
        thisProjection = new ProjectionOrthographic(flipped, width, height);
        break;
    case PETERS:
        thisProjection = new ProjectionPeters(flipped, width, height);
        break;
    case RECTANGULAR:
        thisProjection = new ProjectionRectangular(flipped, width, height);
        break;
    default:
	xpWarn("getProjection: Unknown projection type specified\n",
	       __FILE__, __LINE__);
        thisProjection = new ProjectionRectangular(flipped, width, height);
        break;
    }
    return(thisProjection);
}
