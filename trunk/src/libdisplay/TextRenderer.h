#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <string>
#include <vector>

class DisplayBase;

class TextRenderer
{
 public:
    TextRenderer(DisplayBase *display);
    virtual ~TextRenderer();

    void DrawOutlinedText(const int x, int y, 
                          const std::string &text, 
                          const unsigned char color[3]);

    virtual void Font(const std::string &font) = 0;
    const std::string & Font() const { return(font_); };

    virtual void FontSize(const int size) = 0;
    int FontSize() const { return(fontSize_); };

    virtual void DrawText(const int x, const int y, 
                          const unsigned char color[3]) = 0;

    virtual void SetText(const std::string &text) = 0;
    virtual void FreeText() = 0;

    virtual void TextBox(int &textWidth, int &textHeight) = 0;

    virtual int FontHeight() const = 0;

    bool CheckUnicode(const unsigned long unicode, 
                      const std::vector<unsigned char> &text);

    unsigned long UTF8ToUnicode(const std::vector<unsigned char> &text);

    void drawUTF8test();

 protected:

    std::string font_;
    int fontSize_;

    DisplayBase *display_;
    
 private:
};

#endif
