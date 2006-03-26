#include <cstdio>
#include <cstring>
#include <sstream>
using namespace std;

#include <QuickTime/QuickTime.h>

#include "config.h"
#include "xpUtil.h"

// This code is based on the QTtoCG example at
// http://developer.apple.com/samplecode/QTtoCG/QTtoCG.html
typedef struct {
    size_t width;
    size_t height;
    size_t bitsPerComponent;
    size_t bitsPerPixel;
    size_t bytesPerRow;
    size_t size;
    CGImageAlphaInfo ai;
    CGColorSpaceRef cs;
    unsigned char *data;
    CMProfileRef prof;
} BitmapInfo;

static bool
readBitmapInfo(GraphicsImportComponent gi, BitmapInfo *bi)
{
    ImageDescriptionHandle imageDescH = NULL;
    ComponentResult result = GraphicsImportGetImageDescription(gi, 
                                                               &imageDescH);
    if( noErr != result || imageDescH == NULL ) 
    {
        xpWarn("Error while retrieving image description", 
               __FILE__, __LINE__);
        return(false);
    }
        
    ImageDescription *desc = *imageDescH;
        
    bi->width = desc->width;
    bi->height = desc->height;
    bi->bitsPerComponent = 8;
    bi->bitsPerPixel = 32;
    bi->bytesPerRow = (bi->bitsPerPixel * bi->width + 7)/8;
    bi->ai = (desc->depth == 32) ? kCGImageAlphaFirst : kCGImageAlphaNoneSkipFirst;
    bi->size = bi->bytesPerRow * bi->height;
    bi->data = (unsigned char *) malloc(bi->size);
        
    bi->cs = NULL;
    bi->prof = NULL;

    Handle profile = NULL;
    GraphicsImportGetColorSyncProfile(gi, &profile);
    if( NULL != profile ) 
    {
        CMError err;
        CMProfileLocation profLoc;
        Boolean bValid, bPreferredCMMNotFound;
                
        profLoc.locType = cmHandleBasedProfile;
        profLoc.u.handleLoc.h = profile;
                
        err = CMOpenProfile(&bi->prof, &profLoc);
        if( err != noErr ) 
        {
            xpWarn("Cannot open profile\n", __FILE__, __LINE__);
            return(false);
        }
                
        /* Not necessary to validate profile, but good for debugging */
        err = CMValidateProfile(bi->prof, &bValid, &bPreferredCMMNotFound);
        if( err != noErr ) 
        {
            ostringstream errStr;
            errStr << "Cannot validate profile : Valid: " << bValid 
                   << ", Preferred CMM not found : " 
                   << bPreferredCMMNotFound << "\n";
            xpWarn(errStr.str(), __FILE__, __LINE__);
            return(false);
        }
                
        bi->cs = CGColorSpaceCreateWithPlatformColorSpace( &bi->prof );
                
        if( bi->cs == NULL ) 
        {
            xpWarn("Error creating cg colorspace from csync profile", 
                   __FILE__, __LINE__);
            return(false);
        }
        xpMsg("Embedded profile found in image\n", __FILE__, __LINE__);
        DisposeHandle(profile);
    }  
        
    if( imageDescH != NULL)
        DisposeHandle((Handle)imageDescH);
}

static bool
getBitmapData(GraphicsImportComponent gi, BitmapInfo *bi)
{
    GWorldPtr gWorld;
    QDErr err = noErr;
    Rect boundsRect = { 0, 0, bi->height, bi->width };
    ComponentResult result;
        
    if( bi->data == NULL ) 
    {
        xpWarn("no bitmap buffer available\n", __FILE__, __LINE__);
        return(false);
    }
        
    err = NewGWorldFromPtr( &gWorld, k32ARGBPixelFormat, &boundsRect, 
                            NULL, NULL, 0, (char *) bi->data, 
                            bi->bytesPerRow );
    if (noErr != err) 
    {
        ostringstream errStr;
        errStr << "error creating new gworld - " << err << "\n";
        xpWarn(errStr.str(), __FILE__, __LINE__);
        return(false);
    }
        
    if( (result = GraphicsImportSetGWorld(gi, gWorld, NULL)) != noErr ) 
    {
        xpWarn("error while setting gworld\n", __FILE__, __LINE__);
        return(false);
    }
        
    if( (result = GraphicsImportDraw(gi)) != noErr ) 
    {
        xpWarn("error while drawing image through qt\n", __FILE__, __LINE__);
        return(false);
    }
        
    DisposeGWorld(gWorld);  
}

bool 
ReadImage(const char *filename, int &width, int &height, 
          unsigned char *&rgb_data, unsigned char *&png_alpha)
{
    FSRef ref;
    OSStatus status = FSPathMakeRef((const UInt8 *) filename, &ref, NULL);
    if (status != noErr) 
    {
        ostringstream errStr;
        errStr << "Can't make FSRef for " << filename << "\n";
        xpWarn(errStr.str(), __FILE__, __LINE__);
        return(false);
    }
        
    FSSpec spec;
    OSErr err = FSGetCatalogInfo(&ref, kFSCatInfoNone, NULL, NULL, 
                                 &spec, NULL);
    if (err != noErr) 
    {
        ostringstream errStr;
        errStr << "Can't make FSSpec for " << filename << "\n";
        xpWarn(errStr.str(), __FILE__, __LINE__);
        return(false);
    }

    GraphicsImportComponent gi;
    GetGraphicsImporterForFile(&spec, &gi);
    if (gi == NULL)
    {
        ostringstream errStr;
        errStr << "Can't get GraphicsImporter for " << filename << "\n";
        xpWarn(errStr.str(), __FILE__, __LINE__);
        return(false);
    }

    BitmapInfo bi;

    if (!readBitmapInfo(gi, &bi) 
        || !getBitmapData(gi, &bi)) 
    {
        ostringstream errStr;
        errStr << "Can't get bitmap for " << filename << "\n";
        xpWarn(errStr.str(), __FILE__, __LINE__);
        return(false);
    }
    
    CloseComponent(gi);
        
    width = static_cast<int> (bi.width);
    height = static_cast<int> (bi.height);
    int area = width * height;

    rgb_data = (unsigned char *) malloc(3 * area);
    if (bi.ai == kCGImageAlphaFirst)
    {
        png_alpha = (unsigned char *) malloc(area);
        unsigned char *rgbPtr = rgb_data;
        unsigned char *pngPtr = png_alpha;
        unsigned char *bitPtr = bi.data;
        for (int i = 0; i < area; i++)
        {
            *pngPtr++ = *bitPtr++;
            memcpy(rgbPtr, bitPtr, 3);
            rgbPtr += 3;
            bitPtr += 3;
        }
    }
    else // no alpha channel
    {
        unsigned char *rgbPtr = rgb_data;
        unsigned char *bitPtr = bi.data;
        bitPtr++;
        for (int i = 0; i < area; i++)
        {
            memcpy(rgbPtr, bitPtr, 3);
            rgbPtr += 3;
            bitPtr += 4;
        }
    }
    free(bi.data);

    return(true);
}
