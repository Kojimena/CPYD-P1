#include <stdio.h>
#include <SDL.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// Función para inicializar una figura
typedef struct {
    int x, y;           // Posición de la figura
    int width, height;  // Dimensiones de la figura
    int speedX, speedY; // Velocidad en los ejes X e Y
    Uint32 color;       // Color de la figura
} Figura;


Figura createFigura(int x, int y, int width, int height, int speedX, int speedY, Uint32 color) {
    Figura figura;
    figura.x = x;
    figura.y = y;
    figura.width = width;
    figura.height = height;
    figura.speedX = speedX;
    figura.speedY = speedY;
    figura.color = color;
    return figura;
}

// Función para mover una figura
void moveFigura(Figura* figura) {
    figura->x += figura->speedX;
    figura->y += figura->speedY;

    // Cambiar dirección si toca los bordes de la pantalla
    if (figura->x <= 0 || figura->x >= (SCREEN_WIDTH - figura->width)) {
        figura->speedX *= -1;
    }
    if (figura->y <= 0 || figura->y >= (SCREEN_HEIGHT - figura->height)) {
        figura->speedY *= -1;
    }
}

// Función para dibujar una figura en la superficie dada
void drawFigura(SDL_Surface* surface, Figura* figura) {
    SDL_Rect rect = {figura->x, figura->y, figura->width, figura->height};
    SDL_FillRect(surface, &rect, figura->color);
}

// Función para detectar colisión entre dos figuras
// Función para detectar colisión entre dos figuras y manejar la respuesta
// Función para detectar colisión entre dos figuras
int checkCollision(Figura* a, Figura* b) {
    int collisionX = a->x < b->x + b->width && a->x + a->width > b->x;
    int collisionY = a->y < b->y + b->height && a->y + a->height > b->y;
    return collisionX && collisionY;
}

