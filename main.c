#include <stdio.h>
#include <SDL.h>

#include "figure.h"
#include "color.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FPS 60

// Función para inicializar SDL y crear una ventana
SDL_Window* initializeWindow(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
    }

    return window;
}

// Función para cerrar SDL y destruir la ventana
void cleanup(SDL_Window* window) {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    SDL_Window* window = initializeWindow("Screen Saver", SCREEN_WIDTH, SCREEN_HEIGHT);
    if (window == NULL) {
        return 1;
    }

    SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

    // Inicializar colores
    Uint32 redColor = RED(screenSurface);
    Uint32 blueColor = BLUE(screenSurface);

    // Crear figuras
    Figura figura1 = createFigura(0, (SCREEN_HEIGHT - 50) / 2, 50, 50, 5, 0, redColor);
    Figura figura2 = createFigura(SCREEN_WIDTH - 50, (SCREEN_HEIGHT - 50) / 2, 50, 50, -5, 0, blueColor);

    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Mover figuras
        moveFigura(&figura1);
        moveFigura(&figura2);

        // Detectar colisiones entre las figuras
        if (checkCollision(&figura1, &figura2)) {
            figura1.speedX *= -1;
            figura2.speedX *= -1;
        }

        // Borrar la pantalla
        SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));

        // Dibujar las figuras
        drawFigura(screenSurface, &figura1);
        drawFigura(screenSurface, &figura2);

        SDL_UpdateWindowSurface(window);
        SDL_Delay(1000 / FPS);
    }

    cleanup(window);

    return 0;
}

