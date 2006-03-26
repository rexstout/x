#ifndef IMAGE_H
#define IMAGE_H

class Image
{
 public:
    Image();
    Image(const int w, const int h, const unsigned char *rgb, 
	  const unsigned char *alpha);

    ~Image();

    const unsigned char * getPNGAlpha() const { return(png_alpha); };
    const unsigned char * getRGBData() const { return(rgb_data); };

    void getPixel(double px, double py, unsigned char *pixel);
    void getPixel(double px, double py, unsigned char *pixel, 
		  unsigned char *alpha);

    int Width() const  { return(width); };
    int Height() const { return(height); };
    void Quality(const int q) { quality_ = q; };

    bool Read(const char *filename);
    bool Write(const char *filename);

    void Reduce(const int factor);
    void Resize(const int w, const int h);

 private:
    int width, height, area;
    unsigned char *rgb_data;
    unsigned char *png_alpha;

    int quality_;
};

#endif
