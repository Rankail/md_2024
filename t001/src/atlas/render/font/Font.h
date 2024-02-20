#ifndef MD_2024PROJ_FONT_H
#define MD_2024PROJ_FONT_H

#include <string>
#include <SDL_ttf.h>

#include "utils.h"

class Font {
private:
    TTF_Font* font;

public:
    Font(const std::string& path, unsigned fontSize);
    virtual ~Font();

private:
    static TTF_Font* loadFontFromFile(const std::string& filePath, unsigned fontSize);

public:
    TTF_Font* getFont() const { return font; }
};

#endif //MD_2024PROJ_FONT_H
