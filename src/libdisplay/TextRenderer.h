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

    void DrawText(const int x, int y, 
                  const std::string &text, 
                  const unsigned char color[3]);

    void DrawOutlinedText(const int x, int y, 
                          const std::string &text, 
                          const unsigned char color[3]);

    virtual void Font(const std::string &font);
    const std::string & Font() const { return(font_); };

    virtual void FontSize(const int size);
    int FontSize() const { return(fontSize_); };

    virtual void DrawText(const int x, const int y, 
                          const unsigned char color[3]);

    virtual void SetText(const std::string &text);
    virtual void FreeText();

    virtual void TextBox(int &textWidth, int &textHeight);

    virtual int FontHeight() const;

    bool CheckUnicode(const unsigned long unicode, 
                      const std::vector<unsigned char> &text);

    unsigned long UTF8ToUnicode(const std::vector<unsigned char> &text);

 protected:

    std::string font_;
    int fontSize_;

    DisplayBase *display_;
    
 private:
};

#endif
