#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

#include "Image.h"

#include "config.h"

extern "C"
{
    int
    read_bmp(const char *filename, int *width, int *height, unsigned char **rgb);
    int
    write_bmp(const char *filename, int width, int height, unsigned char *rgb);
    
#ifdef HAVE_LIBGIF
    int
    read_gif(const char *filename, int *width, int *height, unsigned char **rgb);
    int
    write_gif(const char *filename, int width, int height, unsigned char *rgb);
#endif
    
#ifdef HAVE_LIBJPEG
    int
read_jpeg(const char *filename, int *width, int *height, unsigned char **rgb);
    int
    write_jpeg(FILE *outfile, int width, int height, unsigned char *rgb, 
	       int quality);
#endif
    
#ifdef HAVE_LIBPNG
    int
    read_png(const char *filename, int *width, int *height, unsigned char **rgb, 
	     unsigned char **alpha);
    int
    write_png(FILE *outfile, int width, int height, unsigned char *rgb, 
	      unsigned char *alpha);
#endif
    
#ifdef HAVE_LIBPNM
#include <pnm.h>
    int
    read_pnm(const char *filename, int *width, int *height, unsigned char **rgb);
    int
    write_pnm(FILE *outfile, int width, int height, unsigned char *rgb,
	      int maxv, int format, int forceplain);
#endif
    
#ifdef HAVE_LIBTIFF
    int
    read_tiff(const char *filename, int *width, int *height, unsigned char **rgb);
    int
    write_tiff(const char *filename, int width, int height, unsigned char *rgb);
#endif
}

Image::Image() : width(0), height(0), area(0), 
		 rgb_data(NULL), png_alpha(NULL), quality_(80)
{
}

Image::Image(const int w, const int h, const unsigned char *rgb, const unsigned char *alpha) :
    width(w), height(h), area(w*h), quality_(80)
{
    width = w;
    height = h;
    area = w * h;
    
    rgb_data = (unsigned char *) malloc(3 * area);
    memcpy(rgb_data, rgb, 3 * area);

    if (alpha == NULL)
    {
	png_alpha = NULL;
    }
    else
    {
	png_alpha = (unsigned char *) malloc(area);
	memcpy(png_alpha, alpha, area);
    }
}

Image::~Image()
{
    free(rgb_data);
    free(png_alpha);
}

bool
Image::Read(const char *filename)
{
    char buf[4];
    unsigned char *ubuf = (unsigned char *) buf;
    int success = 0;

    FILE *file;
    file = fopen(filename, "rb");
    if (file == NULL) return(false);
  
    /* see what kind of file we have */

    fread(buf, 1, 4, file);
    fclose(file);

    if (!strncmp("BM", buf, 2))
    {
        success = read_bmp(filename, &width, &height, &rgb_data);
    }
    else if (!strncmp("GIF8", buf, 4))
    {
#ifdef HAVE_LIBGIF
        success = read_gif(filename, &width, &height, &rgb_data);
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with GIF support\n");
        success = 0;
#endif /* HAVE_LIBGIF */
    }
    else if ((ubuf[0] == 0xff) && (ubuf[1] == 0xd8))
    {
#ifdef HAVE_LIBJPEG
        success = read_jpeg(filename, &width, &height, &rgb_data);
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with JPEG support\n");
        success = 0;
#endif /* HAVE_LIBJPEG */
    }
    else if ((ubuf[0] == 0x89) && !strncmp("PNG", buf+1, 3))
    {
#ifdef HAVE_LIBPNG
        success = read_png(filename, &width, &height, &rgb_data, &png_alpha);
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with PNG support\n");
        success = 0;
#endif /* HAVE_LIBPNG */
    }
    else if ((   !strncmp("P6\n", buf, 3))
             || (!strncmp("P5\n", buf, 3))
             || (!strncmp("P4\n", buf, 3))
             || (!strncmp("P3\n", buf, 3))
             || (!strncmp("P2\n", buf, 3))
             || (!strncmp("P1\n", buf, 3)))
    {
#ifdef HAVE_LIBPNM
        success = read_pnm(filename, &width, &height, &rgb_data);
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with PNM support\n");
        success = 0;
#endif /* HAVE_LIBPNM */
    }
    else if (((!strncmp ("MM", buf, 2)) && (ubuf[2] == 0x00) 
              && (ubuf[3] == 0x2a))
             || ((!strncmp ("II", buf, 2)) && (ubuf[2] == 0x2a) 
                 && (ubuf[3] == 0x00)))
    {
#ifdef HAVE_LIBTIFF
        success = read_tiff(filename, &width, &height, &rgb_data);
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with TIFF support\n");
        success = 0;
#endif
    }
    else
    {
        fprintf(stderr, "Unknown image format\n");
        success = 0;
    }

    return(success == 1);
}

bool
Image::Write(const char *filename)
{
    FILE *outfile;
    const char *extension = strrchr(filename, '.');
    char *lowercase;
    char *ptr;
    int success = 0;
  
    lowercase = (char *) malloc(strlen(extension) + 1);
    strcpy(lowercase, extension);
    ptr = lowercase;

    while (*ptr != '\0') *ptr++ = (char) tolower(*extension++);

    outfile = fopen(filename, "wb");
    if (outfile == NULL) return(false);
  
    if (strcmp(lowercase, ".bmp" ) == 0)
    {
        success = write_bmp(filename, width, height, rgb_data); 
    }
    else if (strcmp(lowercase, ".gif" ) == 0)
    {
#ifdef HAVE_LIBGIF
        success = write_gif(filename, width, height, rgb_data); 
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with GIF support\n");
        success = 0;
#endif /* HAVE_LIBPNG */
    }
    else if ((   strcmp(lowercase, ".jpg" ) == 0)
             || (strcmp(lowercase, ".jpeg") == 0))
    {
#ifdef HAVE_LIBJPEG
        success = write_jpeg(outfile, width, height, rgb_data, quality_); 
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with JPEG support\n");
        success = 0;
#endif /* HAVE_LIBJPEG */
    }

    else if (strcmp(lowercase, ".png" ) == 0)
    {
#ifdef HAVE_LIBPNG
        success = write_png(outfile, width, height, rgb_data, png_alpha); 
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with PNG support\n");
        success = 0;
#endif /* HAVE_LIBPNG */
    }

    else if ((   strcmp(lowercase, ".pbm") == 0)
             || (strcmp(lowercase, ".pgm") == 0)
             || (strcmp(lowercase, ".ppm") == 0))
    {
#ifdef HAVE_LIBPNM
        if (strcmp(lowercase, ".pbm") == 0)
            success = write_pnm(outfile, width, height, rgb_data, 1, PBM_TYPE, 0);
        else if (strcmp(lowercase, ".pgm") == 0)
            success = write_pnm(outfile, width, height, rgb_data, 255, 
                                PGM_TYPE, 0);
        else if (strcmp(lowercase, ".ppm") == 0)
            success = write_pnm(outfile, width, height, rgb_data, 255, 
                                PPM_TYPE, 0);
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with PNM support\n");
        success = 0;
#endif /* HAVE_LIBPNM */
    }

    else if ((strcmp(lowercase, ".tif" ) == 0)
             || (strcmp(lowercase, ".tiff" ) == 0))
    {
#ifdef HAVE_LIBTIFF
        success = write_tiff(filename, width, height, rgb_data); 
#else
        fprintf(stderr, 
                "Sorry, this program was not compiled with TIFF support\n");
        success = 0;
#endif /* HAVE_LIBTIFF */
    }

    else
    {
        fprintf(stderr, "Unknown image format\n");
        success = 0;
    }

    free(lowercase);
    fclose(outfile);

    return(success == 1);
}

void
Image::Reduce(const int factor)
{
    if (factor < 1) return;

    int scale = 1;
    for (int i = 0; i < factor; i++) scale *= 2;

    double scale2 = scale*scale;

    int w = width / scale;
    int h = height / scale;
    int new_area = w * h;

    unsigned char *new_rgb = (unsigned char *) malloc(3 * new_area);
    memset(new_rgb, 0, 3 * new_area);

    unsigned char *new_alpha = NULL;
    if (png_alpha != NULL)
    {
	new_alpha = (unsigned char *) malloc(new_area);
	memset(new_alpha, 0, new_area);
    }

    int ipos = 0;
    for (int j = 0; j < height; j++)
    {
	int js = j / scale;
	for (int i = 0; i < width; i++)
	{
	    int is = i/scale;
	    for (int k = 0; k < 3; k++)
		new_rgb[3*(js * w + is) + k] += static_cast<unsigned char> ((rgb_data[3*ipos + k] + 0.5) / scale2);

	    if (png_alpha != NULL) new_alpha[js * w + is] += static_cast<unsigned char> (png_alpha[ipos]/scale2);
	    ipos++;
	}
    }

    free(rgb_data);
    free(png_alpha);

    rgb_data = new_rgb;
    png_alpha = new_alpha;
    width = w;
    height = h;

    area = w * h;
}

void
Image::Resize(const int w, const int h)
{
    int new_area = w * h;
    
    unsigned char *new_rgb = (unsigned char *) malloc(3 * new_area);
    unsigned char *new_alpha = NULL;
    if (png_alpha != NULL)
	new_alpha = (unsigned char *) malloc(new_area);

    const double scale_x = ((double) w) / width;
    const double scale_y = ((double) h) / height;
    
    int ipos = 0;
    for (int j = 0; j < h; j++)
    {
	const double y = j / scale_y;
	for (int i = 0; i < w; i++)
	{
	    const double x = i / scale_x;
	    if (new_alpha == NULL)
		getPixel(x, y, new_rgb + 3*ipos);
	    else
		getPixel(x, y, new_rgb + 3*ipos, new_alpha + ipos);
	    ipos++;
	}
    }

    free(rgb_data);
    free(png_alpha);

    rgb_data = new_rgb;
    png_alpha = new_alpha;
    width = w;
    height = h;

    area = w * h;
}

// Find the color of the desired point using bilinear interpolation.
// Assume the array indices refer to the denter of the pixel, so each
// pixel has corners at (i - 0.5, j - 0.5) and (i + 0.5, j + 0.5)
void
Image::getPixel(double x, double y, unsigned char *pixel)
{
    getPixel(x, y, pixel, NULL);
}

void
Image::getPixel(double x, double y, unsigned char *pixel, unsigned char *alpha)
{
    if (x < -0.5) x = -0.5;
    if (x >= width - 0.5) x = width - 0.5;

    if (y < -0.5) y = -0.5;
    if (y >= height - 0.5) y = height - 0.5;

    int ix0 = (int) (floor(x));
    int ix1 = ix0 + 1;
    if (ix0 < 0) ix0 = width - 1;
    if (ix1 >= width) ix1 = 0;

    int iy0 = (int) (floor(y));
    int iy1 = iy0 + 1;
    if (iy0 < 0) iy0 = 0;
    if (iy1 >= height) iy1 = height - 1;

    const double t = x - floor(x);
    const double u = 1 - (y - floor(y));

    // Weights are from Numerical Recipes, 2nd Edition
    //        weight[0] = (1 - t) * u;
    //        weight[2] = (1-t) * (1-u);
    //        weight[3] = t * (1-u);
    double weight[4];
    weight[1] = t * u;
    weight[0] = u - weight[1];
    weight[2] = 1 - t - u + weight[1];
    weight[3] = t - weight[1];

    unsigned char *pixels[4];
    pixels[0] = rgb_data + 3 * (iy0 * width + ix0);
    pixels[1] = rgb_data + 3 * (iy0 * width + ix1);
    pixels[2] = rgb_data + 3 * (iy1 * width + ix0);
    pixels[3] = rgb_data + 3 * (iy1 * width + ix1);

    memset(pixel, 0, 3);
    for (int i = 0; i < 4; i++)
    {
	for (int j = 0; j < 3; j++)
	    pixel[j] += (unsigned char) (weight[i] * pixels[i][j]);
    }

    if (alpha != NULL)
    {
	unsigned char pixels[4];
	pixels[0] = png_alpha[iy0 * width + ix0];
	pixels[1] = png_alpha[iy0 * width + ix1];
	pixels[2] = png_alpha[iy0 * width + ix0];
	pixels[3] = png_alpha[iy1 * width + ix1];

	for (int i = 0; i < 4; i++)
	    *alpha = (unsigned char) (weight[i] * pixels[i]);
    }
}
