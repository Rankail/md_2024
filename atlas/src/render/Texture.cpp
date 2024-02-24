#include "atlas/render/Texture.h"

#include "glad/glad.h"
#include "SDL_image.h"

#include "utils/log/Logger.h"

Texture::Texture(const std::string &path) : glID(0) {
    createSurface(path);
}

Texture::~Texture() {
    if (surface != nullptr) {
        SDL_FreeSurface(surface);
        if (glID != 0) {
            glDeleteTextures(1, &glID);
        }
    }
}

void Texture::createSurface(const std::string &path) {
    surface = IMG_Load(path.c_str());
    if (surface == nullptr) {
        TET_CRITICAL("Failed to load texture from '", path , "'.");
    }

    width = surface->w;
    height = surface->h;
}

void Texture::bind() {
    if (!loaded) {
        if (surface == nullptr) {
            TET_CRITICAL("Texture was not loaded");
        }
        glGenTextures(1, &glID);
        glBindTexture(GL_TEXTURE_2D, glID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        if (surface->format->BytesPerPixel == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
        }

        loaded = true;
    }

    glBindTexture(GL_TEXTURE_2D, glID);
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}
