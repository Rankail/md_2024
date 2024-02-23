#include "Font.h"

#include <utils/log/Logger.h>

Font::Font(const std::string& path, unsigned fontSize)
    : font(loadFontFromFile(path, fontSize)) {}

Font::~Font() {
    TTF_CloseFont(font);
}

TTF_Font* Font::loadFontFromFile(const std::string& filePath, unsigned size) {
    TTF_Font* font = TTF_OpenFont(filePath.c_str(), (int)size);
    if (font == nullptr) {
        TET_CRITICAL("Failed to open font '{}'", filePath);
    }

    return font;
}
