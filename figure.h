#pragma once

#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <math.h>

#define SCREEN_WIDTH 5000  ///< Ancho de la pantalla.
#define SCREEN_HEIGHT 3000 ///< Alto de la pantalla.

/**
 * @brief Estructura que representa una figura en la pantalla.
 *
 * La figura tiene una posición, dimensiones, velocidad en los ejes X e Y,
 * un color y una textura asociada.
 */
typedef struct {
    int x, y;           ///< Posición de la figura en los ejes X e Y.
    int width, height;  ///< Dimensiones de la figura (ancho y alto).
    int speedX, speedY; ///< Velocidad de la figura en los ejes X e Y.
    SDL_Color color;    ///< Color de la figura.
    SDL_Texture *texture; ///< Textura de la figura.
} Figura;

/**
 * @brief Carga una textura desde un archivo y la convierte a una textura de SDL.
 *
 * @param filename Ruta del archivo de imagen.
 * @param renderer El renderer de SDL donde se dibujará la textura.
 * @return SDL_Texture* Un puntero a la textura cargada. Retorna NULL si ocurre un error.
 */
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

/**
 * @brief Crea una figura con los parámetros dados y carga su textura.
 *
 * @param x Posición en el eje X.
 * @param y Posición en el eje Y.
 * @param width Ancho de la figura.
 * @param height Alto de la figura.
 * @param speedX Velocidad en el eje X.
 * @param speedY Velocidad en el eje Y.
 * @param image Ruta de la imagen a usar como textura.
 * @param renderer Renderer de SDL para dibujar la figura.
 * @param color Color de la figura.
 * @return Figura Estructura Figura inicializada.
 */
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

/**
 * @brief Dibuja una figura en el renderer proporcionado.
 *
 * @param renderer Renderer de SDL donde se dibujará la figura.
 * @param figura Puntero a la estructura Figura que se desea dibujar.
 */
void drawFigura(SDL_Renderer* renderer, Figura* figura) {
    SDL_Rect rect = {figura->x, figura->y, figura->width, figura->height};
    SDL_SetTextureColorMod(figura->texture, figura->color.r, figura->color.g, figura->color.b);
    SDL_RenderCopy(renderer, figura->texture, NULL, &rect);
}

/**
 * @brief Verifica si hay colisión entre dos figuras.
 *
 * @param a Puntero a la primera figura.
 * @param b Puntero a la segunda figura.
 * @return int Retorna 1 si hay colisión, 0 en caso contrario.
 */
int checkCollision(Figura* a, Figura* b) {
    int collisionX = a->x < b->x + b->width && a->x + a->width > b->x;
    int collisionY = a->y < b->y + b->height && a->y + a->height > b->y;
    return collisionX && collisionY;
}