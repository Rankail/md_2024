#include "atlas/render/font/FontFamily.h"
#include "utils/log/Logger.h"

#include <utility>

FontFamily::FontFamily(const std::string &filePath) {
    fonts.emplace(FontStyle::REGULAR, FontSizes{
        filePath, std::unordered_map<unsigned, Font*>{}
    });
}

Font *FontFamily::getFont(FontStyle style, unsigned fontSize) {
    auto sz = fonts.find(style);
    if (sz == fonts.end()) {
        if (style == FontStyle::REGULAR)
            TET_CRITICAL("Attempt to use font with no regular style.");

        return getFont(FontStyle::REGULAR, fontSize);
    }

    auto size = &sz->second;

    auto fn = size->sizes.find(fontSize);
    if (fn != size->sizes.end()) {
        return fn->second;
    }

    auto font = new Font(size->filePath, fontSize);
    size->sizes.emplace(fontSize, font);
    return font;
}

Font *FontFamily::getFont(unsigned fontSize) {
    return getFont(FontStyle::REGULAR, fontSize);
}

FontFamily::~FontFamily() {
    for (auto& [_, sizes] : fonts) {
        for (auto& [_, font] : sizes.sizes) delete font;
    }
}

void FontFamily::addStyle(FontStyle style, const std::string &filePath) {
    if (fonts.find(style) != fonts.end()) {
        TET_CRITICAL("Attempt to add font style that was already present.");
    }

    fonts.emplace(style, FontSizes{
        filePath, std::unordered_map<unsigned, Font*>{}
    });
}
