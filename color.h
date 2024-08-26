#pragma once

#include <stdio.h>
#include <SDL.h>

Uint32 RED(SDL_Surface *screenSurface) {
    return SDL_MapRGB(screenSurface->format, 0xFF, 0x00, 0x00);
}

Uint32 BLUE(SDL_Surface *screenSurface) {
    return SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0xFF);
}

Uint32 GREEN(SDL_Surface *screenSurface) {
    return SDL_MapRGB(screenSurface->format, 0x00, 0xFF, 0x00);
}