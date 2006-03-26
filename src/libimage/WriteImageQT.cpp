#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
using namespace std;

#include <Quicktime/Quicktime.h>

#include "config.h"
#include "xpUtil.h"

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

bool 
WriteImage(const char *filename, const int width, const int height, 
           unsigned char * const rgb_data, 
           unsigned char * const png_alpha, 
           const int quality)
{
    const char *extension = strrchr(filename, '.');
    char *lowercase = (char *) malloc(strlen(extension) + 1);
    strcpy(lowercase, extension);
    char *ptr = lowercase;

    while (*ptr != '\0') *ptr++ = (char) tolower(*extension++);

    OSType fileType = 0;
    if (strcmp(lowercase, ".bmp") == 0)
    {
        fileType = kQTFileTypeBMP;
    }
    else if (strcmp(lowercase, ".gif") == 0)
    {
        fileType = kQTFileTypeGIF;
    }
    else if ((strcmp(lowercase, ".jpg") == 0)
             ||(strcmp(lowercase, ".jpeg") == 0))
    {
        fileType = kQTFileTypeJPEG;
    }
    else if (strcmp(lowercase, ".png") == 0)
    {
        fileType = kQTFileTypePNG;
    }
    else if ((strcmp(lowercase, ".tif") == 0)
             ||(strcmp(lowercase, ".tiff") == 0))
    {
        fileType = kQTFileTypeTIFF;
    }
    if (fileType == 0)
    {
        ostringstream errStr;
        errStr << "Invalid file type for " << filename << endl;
        return(false);
    }

    BitmapInfo bi;
    bi.width = width;
    bi.height = height;
    bi.bitsPerComponent = 8;
    bi.bitsPerPixel = 32;
    bi.bytesPerRow = (bi.bitsPerPixel * bi.width + 7)/8;
    bi.size = bi.bytesPerRow * bi.height;
    bi.ai = (png_alpha == NULL ? kCGImageAlphaNoneSkipFirst : kCGImageAlphaFirst);
    bi.data = (unsigned char *) malloc(bi.size);

    int area = width * height;
    if (bi.ai == kCGImageAlphaFirst)
    {
        unsigned char *rgbPtr = rgb_data;
        unsigned char *pngPtr = png_alpha;
        unsigned char *bitPtr = bi.data;
        for (int i = 0; i < area; i++)
        {
            *bitPtr++ = *pngPtr++;
            memcpy(bitPtr, rgbPtr, 3);
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
            memcpy(bitPtr, rgbPtr, 3);
            rgbPtr += 3;
            bitPtr += 4;
        }
    }

    //Create a CGImage from the bitmap data using the DeviceRGB color space

    CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();

    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, 
                                                              bi.data, 
                                                              bi.size, 
                                                              NULL);
    CGImageRef imageRef = CGImageCreate(bi.width, bi.height, 
                                        bi.bitsPerComponent, bi.bitsPerPixel,
                                        bi.bytesPerRow, cs, bi.ai, provider,
                                        NULL, 0, kCGRenderingIntentDefault);
    CGColorSpaceRelease(cs);

    Handle                     dataRef = NULL;
    OSType                     dataRefType;
    GraphicsExportComponent    graphicsExporter;
    unsigned long              sizeWritten;
    CFStringRef path = CFStringCreateWithCString(NULL, filename, 
                                                 kCFStringEncodingUTF8);
    ComponentResult result = 
        QTNewDataReferenceFromFullPathCFString(path,
                                               kQTNativeDefaultPathStyle,
                                               0, &dataRef, &dataRefType);
    
    if (result != noErr)
    {
        ostringstream errStr;
        errStr << "Can't create data reference for " << filename << "\n";
        errStr << "Did you specify the full path?\n";
        xpWarn(errStr.str(), __FILE__, __LINE__);
        return(false);
    }

    result = OpenADefaultComponent(GraphicsExporterComponentType,
                                   fileType, &graphicsExporter);
    if (result != noErr)
    {
        ostringstream errStr;
        errStr << "Can't create GraphicsExporter for " << filename << "\n";
        xpWarn(errStr.str(), __FILE__, __LINE__);
        return(false);
    }

    result = GraphicsExportSetInputCGImage(graphicsExporter, imageRef);
    if (result != noErr)
    {
        ostringstream errStr;
        errStr << "Can't set input for " << filename << "\n";
        xpWarn(errStr.str(), __FILE__, __LINE__);
        return(false);
    }

    result = GraphicsExportSetOutputDataReference(graphicsExporter,
                                                  dataRef, dataRefType);
    if (result != noErr)
    {
        ostringstream errStr;
        errStr << "Can't set output for " << filename << "\n";
        xpWarn(errStr.str(), __FILE__, __LINE__);
        return(false);
    }

    result = GraphicsExportDoExport(graphicsExporter, &sizeWritten);
    if (result != noErr)
    {
        ostringstream errStr;
        errStr << "Can't export image for " << filename << "\n";
        xpWarn(errStr.str(), __FILE__, __LINE__);
        return(false);
    }

    CloseComponent(graphicsExporter);
    DisposeHandle(dataRef);
    CGImageRelease(imageRef);
    CGDataProviderRelease(provider);

    free(bi.data);

    return(true);
}
