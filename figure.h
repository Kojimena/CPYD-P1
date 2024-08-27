#pragma once

#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <math.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Función para inicializar una figura
typedef struct {
    int x, y;           // Posición de la figura
    int width, height;  // Dimensiones de la figura
    int speedX, speedY; // Velocidad en los ejes X e Y
    SDL_Color color;       // Color de la figura
    SDL_Texture *texture;
} Figura;

SDL_Texture* loadTexture(const char* filename, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(filename);
    if (surface == NULL) {
        printf("Error al cargar la imagen %s: %s\n", filename, IMG_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (texture == NULL) {
        printf("Error al crear la textura para %s: %s\n", filename, SDL_GetError());
    } else {
        printf("Textura cargada correctamente: %s\n", filename);
    }
    return texture;
}



Figura createFigura(int x, int y, int width, int height, int speedX, int speedY, const char* image, SDL_Renderer* renderer, SDL_Color color) {
    Figura figura;
    figura.x = x;
    figura.y = y;
    figura.width = width;
    figura.height = height;
    figura.speedX = speedX;
    figura.speedY = speedY;
    figura.texture = loadTexture(image, renderer);
    figura.color = color;

    if (figura.texture == NULL) {
        printf("No se pudo cargar la figura en (%d, %d)\n", x, y);
    }

    return figura;
}


// Función para dibujar una figura en la superficie dada
void drawFigura(SDL_Renderer* renderer, Figura* figura) {
    SDL_Rect rect = {figura->x, figura->y, figura->width, figura->height};
    SDL_SetTextureColorMod(figura->texture, figura->color.r, figura->color.g, figura->color.b);
    SDL_RenderCopy(renderer, figura->texture, NULL, &rect);
}

// Función para detectar colisión entre dos figuras y el eje de colisión
int checkCollision(Figura* a, Figura* b) {
    int collisionX = a->x < b->x + b->width && a->x + a->width > b->x;
    int collisionY = a->y < b->y + b->height && a->y + a->height > b->y;
    return collisionX && collisionY;
}




