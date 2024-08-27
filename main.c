#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include "figure.h"
#define SCREEN_WIDTH 980
#define SCREEN_HEIGHT 640
#define FPS 60

#define N 5

Figura figuras[N];

// inicializar figuras
void initFiguras(SDL_Renderer *renderer) {
    for (int i = 0; i < N; i++) {
        int x = rand() % (SCREEN_WIDTH - 50);  // [0, SCREEN_WIDTH - 50]
        int y = rand() % (SCREEN_HEIGHT - 50);  // [0, SCREEN_HEIGHT - 50]

        int spd_x = rand() % 10 - 5;  // [-5, 5]
        int spd_y = rand() % 10 - 5;  // [-5, 5]

        char* image = "assets/dvd_logo.png";

        SDL_Color color = {rand() % 256, rand() % 256, rand() % 256, 255};

        figuras[i] = createFigura(x, y, 64, 38, spd_x, spd_y, image, renderer, color);
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
        return NULL;
    }

    return window;
}

//

// Función para cerrar SDL y destruir la ventana
void cleanup(SDL_Window* window) {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = initializeWindow("Screen Saver", SCREEN_WIDTH, SCREEN_HEIGHT);
    if (window == NULL) {
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

    // Inicializar figuras
    initFiguras(renderer);

    Uint32 startTicks, endTicks;
    float fps = 0.0f;
    char title[100];

    int running = 1;
    while (running) {
        startTicks = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        SDL_RenderClear(renderer);

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

        // Dibujar las figuras
        for (int i = 0; i < N; i++) {
            drawFigura(renderer, &figuras[i]);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / FPS);

        endTicks = SDL_GetTicks();
        fps = 1000.0f / (endTicks - startTicks);
        snprintf(title, sizeof(title), "Screen Saver - FPS: %.2f", fps);
        SDL_SetWindowTitle(window, title);
    }

    cleanup(window);

    return 0;
}

