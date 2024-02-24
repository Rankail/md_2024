#ifndef MD_2024_FONTFAMILY_H
#define MD_2024_FONTFAMILY_H

#include <string>
#include <unordered_map>
#include "Font.h"
#include "atlas/units/FontStyle.h"

class FontFamily {
private:
    struct FontSizes {
        std::string filePath;
        std::unordered_map<unsigned, Font*> sizes;
    };

    std::unordered_map<FontStyle, FontSizes> fonts;

public:
    explicit FontFamily(const std::string &filePath);
    virtual ~FontFamily();

    Font* getFont(unsigned fontSize);
    Font* getFont(FontStyle style, unsigned fontSize);

    void addStyle(FontStyle style, const std::string &filePath);
};

#endif
