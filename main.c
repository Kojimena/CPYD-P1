#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>

#include "figure.h"
#include "explosion.h"


#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define FPS 120

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

int N = 2;
Figura* figuras;

int E = 0;  // Número de explosiones
Explosion * explosion;  // Arreglo de figuras de explosiones
#define EXPLOSION_FRAMES 10

// Verificar si dos figuras se superponen
int isOverlapping(Figura* a, Figura* b) {
    return (a->x < b->x + b->width && a->x + a->width > b->x &&
            a->y < b->y + b->height && a->y + a->height > b->y);
}

// Inicializar figuras asegurando que no estén superpuestas
void initFiguras(SDL_Renderer *renderer) {
    figuras = (Figura*)malloc(N * sizeof(Figura)); // Asignar memoria para N figuras

    for (int i = 0; i < N; i++) {
        int x, y, spd_x, spd_y;
        SDL_Color color = {rand() % 256, rand() % 256, rand() % 256, 255};
        char* image = "assets/dvd_logo.png";
        Figura nuevaFigura;

        int overlapping;
        do {
            // Generar posición y velocidad aleatoria
            x = rand() % (SCREEN_WIDTH - 64);  // [0, SCREEN_WIDTH - figura_width]
            y = rand() % (SCREEN_HEIGHT - 38);  // [0, SCREEN_HEIGHT - figura_height]

            // [-2, 2]
            spd_x = rand() % 5 - 2;  // spd_x = rand() % 5 - 2
            spd_y = rand() % 5 - 2;  // spd_y = rand() % 5 - 2

            // Crear la nueva figura temporalmente
            nuevaFigura = createFigura(x, y, 64, 38, spd_x, spd_y, image, renderer, color);

            // Verificar si se superpone con alguna figura existente
            overlapping = 0;
            for (int j = 0; j < i; j++) {
                if (isOverlapping(&nuevaFigura, &figuras[j])) {
                    overlapping = 1;
                    break;
                }
            }
        } while (overlapping);  // Repetir hasta que no haya superposición

        // Guardar la figura generada sin superposición
        figuras[i] = nuevaFigura;
    }
}

// Añadir una figura a la lista de figuras
void spawnFigura(SDL_Renderer *renderer) {
    int x, y, spd_x, spd_y;
    SDL_Color color = {rand() % 256, rand() % 256, rand() % 256, 255};
    char* image = "assets/dvd_logo.png";
    Figura nuevaFigura;

    int overlapping;

    do {
        // Generar posición y velocidad aleatoria
        x = rand() % (SCREEN_WIDTH - 64);  // [0, SCREEN_WIDTH - figura_width]
        y = rand() % (SCREEN_HEIGHT - 38);  // [0, SCREEN_HEIGHT - figura_height]
        spd_x = rand() % 10 - 5;  // [-5, 5]
        spd_y = rand() % 10 - 5;  // [-5, 5]

        // Crear la nueva figura temporalmente
        nuevaFigura = createFigura(x, y, 64, 38, spd_x, spd_y, image, renderer, color);

        // Verificar si se superpone con alguna figura existente
        overlapping = 0;
        for (int j = 0; j < N; j++) {
            if (isOverlapping(&nuevaFigura, &figuras[j])) {
                overlapping = 1;
                break;
            }
        }
    } while (overlapping);  // Repetir hasta que no haya superposición

    // Aumentar el tamaño del arreglo de figuras
    figuras = (Figura*)realloc(figuras, (N + 1) * sizeof(Figura));

    // Guardar la nueva figura
    figuras[N] = nuevaFigura;

    // Incrementar el contador de figuras
    N++;
}

// Crea una explosión en la posición (x, y), que previamente era la posición de una figura
void spawnExplosion(SDL_Renderer *renderer, int x, int y) {
    int spd_x, spd_y;
    SDL_Color color = {rand() % 256, rand() % 256, rand() % 256, 255};
    char* image = "assets/explosion.png";
    Explosion nuevaExplosion;

    // Las explosiones no se mueven
    spd_x = 0;
    spd_y = 0;

    nuevaExplosion.figura = createFigura(x, y, 128, 76, spd_x, spd_y, image, renderer, color);
    nuevaExplosion.frames = EXPLOSION_FRAMES;

    explosion = (Explosion*)realloc(explosion, (E + 1) * sizeof(Explosion));
    explosion[E] = nuevaExplosion;
    E++;
}

void cleanExplosions() {
    for (int i = 0; i < E; i++) {
        explosion[i].frames--;
        if (explosion[i].frames == 0) {
            explosion[i] = explosion[E - 1];
            explosion = (Explosion*)realloc(explosion, (E - 1) * sizeof(Explosion));
            E--;
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

void resolveCollision(Figura* a, Figura* b) {
    // Invertir velocidades
    a->speedX *= -1;
    b->speedX *= -1;
    a->speedY *= -1;
    b->speedY *= -1;

    int overlapX = min(a->x + a->width, b->x + b->width) - max(a->x, b->x);
    int overlapY = min(a->y + a->height, b->y + b->height) - max(a->y, b->y);

    if (overlapX < overlapY) {
        int shift = overlapX / 2 + 3;
        a->x += a->speedX > 0 ? shift : -shift;
        b->x += b->speedX > 0 ? -shift : shift;
    } else {
        int shift = overlapY / 2 + 3;
        a->y += a->speedY > 0 ? shift : -shift;
        b->y += b->speedY > 0 ? -shift : shift;
    }
}

// Función para mover una figura
void moveFigura(Figura* figura, SDL_Renderer* renderer) {
    figura->x += figura->speedX;
    figura->y += figura->speedY;

    // Cambiar dirección si toca los bordes de la pantalla
    if (figura->x <= 0 || figura->x >= (SCREEN_WIDTH - figura->width)) {
        figura->speedX *= -1;
        spawnFigura(renderer);
    }
    if (figura->y <= 0 || figura->y >= (SCREEN_HEIGHT - figura->height)) {
        figura->speedY *= -1;
        spawnFigura(renderer);
    }
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

        if (N == 0) {
            break;
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Limpiar explosiones
        cleanExplosions();

        SDL_RenderClear(renderer);

        // Mover figuras
        for (int i = 0; i < N; i++) {
            moveFigura(&figuras[i], renderer);
        }

        // Detectar colisiones entre las figuras
        for (int i = 0; i < N; i++) {
            for (int j = i + 1; j < N; j++) {
                if (checkCollision(&figuras[i], &figuras[j])) {
                    resolveCollision(&figuras[i], &figuras[j]);

                    // decidir al azar cual de las dos figuras se elimina
                    int randNum;

                    if (N <= 2) {
                        randNum = rand() % 2;
                    } else {
                        randNum = rand() % 3;
                    }

                    if (randNum == 0) {
                        int prevX = figuras[i].x;
                        int prevY = figuras[i].y;

                        // Eliminar la figura i
                        figuras[i] = figuras[N - 1];
                        figuras = (Figura*)realloc(figuras, (N - 1) * sizeof(Figura));
                        N--;

                        // Crear una explosión en la posición de la figura eliminada
                        spawnExplosion(renderer, prevX, prevY);
                    } else if (randNum == 1) {
                        int prevX = figuras[j].x;
                        int prevY = figuras[j].y;

                        // Eliminar la figura j
                        figuras[j] = figuras[N - 1];
                        figuras = (Figura*)realloc(figuras, (N - 1) * sizeof(Figura));
                        N--;

                        // Crear una explosión en la posición de la figura eliminada
                        spawnExplosion(renderer, prevX, prevY);
                    } else {
                        int prevX1 = figuras[i].x;
                        int prevY1 = figuras[i].y;
                        int prevX2 = figuras[j].x;
                        int prevY2 = figuras[j].y;

                        // Eliminar ambas figuras
                        figuras[i] = figuras[N - 1];
                        figuras[j] = figuras[N - 2];
                        figuras = (Figura*)realloc(figuras, (N - 2) * sizeof(Figura));
                        N -= 2;

                        // Crear una explosión en la posición de la figura eliminada
                        spawnExplosion(renderer, prevX1, prevY1);
                        spawnExplosion(renderer, prevX2, prevY2);
                    }
                }
            }
        }

        // Dibujar las figuras
        for (int i = 0; i < N; i++) {
            drawFigura(renderer, &figuras[i]);
        }

        // Dibujar explosiones
        for (int i = 0; i < E; i++) {
            drawFigura(renderer, &explosion[i]);
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

