#include <stdio.h>
#include <SDL.h>

#include "figure.h"
#include "color.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FPS 60

#define N 2

Figura figuras[N];

// inicializar figuras
void initFiguras(SDL_Surface *screenSurface) {
    for (int i = 0; i < N; i++) {
        // elegir un color aleatorio
        int c = rand() % 3;

        int x = rand() % (SCREEN_WIDTH - 50);  // [0, SCREEN_WIDTH - 50]
        int y = rand() % (SCREEN_HEIGHT - 50);  // [0, SCREEN_HEIGHT - 50]

        int spd_x = rand() % 10 - 5;  // [-5, 5]
        int spd_y = rand() % 10 - 5;  // [-5, 5]

        switch (c) {
            case 0:
                figuras[i] = createFigura(x, y, 50, 50, spd_x, spd_y, RED(screenSurface));
                break;
            case 1:
                figuras[i] = createFigura(x, y, 50, 50, spd_x, spd_y, BLUE(screenSurface));
                break;
            case 2:
                figuras[i] = createFigura(x, y, 50, 50, spd_x, spd_y, GREEN(screenSurface));
                break;

            default:
                figuras[i] = createFigura(x, y, 50, 50, spd_x, spd_y, RED(screenSurface));
                break;

        }
    }
}

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

    // Inicializar figuras
    initFiguras(screenSurface);

    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Mover figuras
        for (int i = 0; i < N; i++) {
            moveFigura(&figuras[i]);
        }

        // Detectar colisiones entre las figuras
        for (int i = 0; i < N; i++) {
            for (int j = i + 1; j < N; j++) {
                if (checkCollision(&figuras[i], &figuras[j])) {
                    figuras[i].speedX *= -1;
                    figuras[j].speedX *= -1;
                    figuras[i].speedY *= -1;
                    figuras[j].speedY *= -1;
                }
            }
        }

        // Borrar la pantalla
        SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));

        // Dibujar las figuras

        for (int i = 0; i < N; i++) {
            drawFigura(screenSurface, &figuras[i]);
        }

        SDL_UpdateWindowSurface(window);
        SDL_Delay(1000 / FPS);
    }

    cleanup(window);

    return 0;
}

