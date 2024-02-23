#ifndef MD_2024_TEXTURE_H
#define MD_2024_TEXTURE_H

#include <string>
#include <SDL.h>

#include <utils/utils.h>
#include "units/Size.h"

class Texture {
private:
    unsigned glID;
    unsigned width, height;
    bool loaded = false;

    SDL_Surface* surface = nullptr;

public:
    explicit Texture(const std::string& path);
    virtual ~Texture();

private:
    void createSurface(const std::string &path);

public:
    void bind();
    void unbind();

public:
    Size getSize() const { return {width, height}; }
    unsigned getWidth() const { return width; }
    unsigned getHeight() const { return height; }
};


#endif //MD_2024_TEXTURE_H
