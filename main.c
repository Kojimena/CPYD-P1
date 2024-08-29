#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>

#include "figure.h"
#include "explosion.h"

#include <omp.h>

#define FPS 120

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define FIGURE_WIDTH 32
#define FIGURE_HEIGHT 19

int N;
Figura* figuras;

int E = 0;  // Número de explosiones
Explosion * explosion;  // Arreglo de figuras de explosiones
int EXPLOSION_FRAMES;

// Verificar si dos figuras se superponen
int isOverlapping(Figura* a, Figura* b) {
    return (a->x < b->x + b->width && a->x + a->width > b->x &&
            a->y < b->y + b->height && a->y + a->height > b->y);
}

// Inicializar figuras asegurando que no estén superpuestas
void initFiguras(SDL_Renderer *renderer) {
    #pragma omp critical
    {
        figuras = (Figura *) malloc(N * sizeof(Figura)); // Asignar memoria para N figuras
    }

    #pragma omp parallel for schedule(dynamic) shared(figuras, renderer)
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
            nuevaFigura = createFigura(x, y, FIGURE_WIDTH, FIGURE_HEIGHT, spd_x, spd_y, image, renderer, color);

            // Verificar si se superpone con alguna figura existente
            overlapping = 0;
            #pragma omp parallel for shared(overlapping)
            for (int j = 0; j < i; j++) {
                if (isOverlapping(&nuevaFigura, &figuras[j])) {
                    #pragma omp atomic write
                    overlapping = 1;
                    break;
                }
            }

        } while (overlapping);  // Repetir hasta que no haya superposición

        // Guardar la figura generada sin superposición
        # pragma omp critical
        {
            figuras[i] = nuevaFigura;
        };
    }
}

// Añadir una figura a la lista de figuras
void spawnFigura(SDL_Renderer *renderer) {
    int x, y, spd_x, spd_y;
    SDL_Color color = {rand() % 256, rand() % 256, rand() % 256, 255};

    #pragma omp parallel for private(x, y, spd_x, spd_y) shared(color) schedule(auto)
    for (int r = 255; r < 256; r += 16) {
        for (int g = 0; g < 256; g += 16) {
            for (int b = 255; b < 256; b += 16) {

                for (int i = 0; i < N; i++) {
                    if (figuras[i].color.r == r){
                        color.r = rand() % 256;
                        #pragma omp flush(color)
                    }

                    if (figuras[i].color.g == g){
                        color.g = rand() % 256;
                        #pragma omp flush(color)
                    }

                    if (figuras[i].color.b == b){
                        color.b = rand() % 256;
                        #pragma omp flush(color)
                    }
                }

            }

        }

    }

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
        nuevaFigura = createFigura(x, y, FIGURE_WIDTH, FIGURE_HEIGHT, spd_x, spd_y, image, renderer, color);

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
    #pragma omp critical
    {
    figuras = (Figura*)realloc(figuras, (N + 1) * sizeof(Figura));
    }

    // Guardar la nueva figura
    figuras[N] = nuevaFigura;

    // Incrementar el contador de figuras
    N++;
}

// Crea una explosión en la posición (x, y), que previamente era la posición de una figura
void spawnExplosion(SDL_Renderer *renderer, int x, int y) {
    int spd_x, spd_y;
    SDL_Color color = {rand() % 256, rand() % 256, rand() % 256, 255};

    #pragma omp parallel for private(x, y, spd_x, spd_y) shared(color) schedule(auto)
    for (int r = 255; r < 256; r += 16) {
        for (int g = 0; g < 256; g += 16) {
            for (int b = 255; b < 256; b += 16) {

                for (int i = 0; i < E; i++) {
                    if (explosion[i].figura.color.r == r) {
                        color.r = rand() % 256;
                        #pragma omp flush(color)
                    }

                    if (explosion[i].figura.color.g == g) {
                        color.g = rand() % 256;
                        #pragma omp flush(color)
                    }

                    if (explosion[i].figura.color.b == b) {
                        color.b = rand() % 256;
                        #pragma omp flush(color)
                    }
                }

            }

        }

    }


    char* image = "assets/explosion.png";
    Explosion nuevaExplosion;

    // Las explosiones no se mueven
    spd_x = 0;
    spd_y = 0;

    nuevaExplosion.figura = createFigura(x, y, FIGURE_WIDTH*4, FIGURE_HEIGHT*4, spd_x, spd_y, image, renderer, color);
    nuevaExplosion.frames = EXPLOSION_FRAMES;
    #pragma omp critical
    {
        explosion = (Explosion *) realloc(explosion, (E + 1) * sizeof(Explosion));
    }
    explosion[E] = nuevaExplosion;
    E++;
}

void cleanExplosions() {
    #pragma omp parallel for schedule(dynamic, 4)
    for (int i = 0; i < E; i++) {
        explosion[i].frames--;
        if (explosion[i].frames == 0) {
            #pragma omp critical
            {
                explosion[i] = explosion[E - 1];
                explosion = (Explosion *) realloc(explosion, (E - 1) * sizeof(Explosion));
                E--;
            }
        } else {
            #pragma omp atomic
            explosion[i].figura.width -= (FIGURE_WIDTH*4)/EXPLOSION_FRAMES;
            #pragma omp atomic
            explosion[i].figura.height -= (FIGURE_HEIGHT*4)/EXPLOSION_FRAMES;
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

    #pragma omp atomic
    a->speedX *= -1;
    #pragma omp atomic
    b->speedX *= -1;
    #pragma omp atomic
    a->speedY *= -1;
    #pragma omp atomic
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
    if (argc > 2) {
        N = atoi(argv[1]);
        EXPLOSION_FRAMES = atoi(argv[2]);

        if (N <= 0) {
            printf("N debe ser un número positivo\n");
            return 1;
        }

        if (EXPLOSION_FRAMES <= 0) {
            printf("E debe ser un número positivo\n");
            return 1;
        }
    } else {
        printf("Usage: %s <N> <E>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen("iter.log", "w");

    if (file == NULL) {
        printf("Error al abrir el archivo\n");
        return 1;
    }

    printf("Argumentos: N = %d, E = %d\n", N, EXPLOSION_FRAMES);

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

    #pragma omp barrier

    Uint32 startTicks, endTicks, logTicks;
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
        #pragma omp parallel for shared(figuras) schedule(dynamic, 4)
        for (int i = 0; i < N; i++) {
            for (int j = i + 1; j < N; j++) {
                if (checkCollision(&figuras[i], &figuras[j])) {
                    resolveCollision(&figuras[i], &figuras[j]);

                    // decidir al azar cual de las dos figuras se elimina
                    int randNum;

                    if (N <= 2) {
                        randNum = rand() % 4;  // 0, 1, 2, 3
                    } else {
                        randNum = rand() % 5;  // 0, 1, 2, 3, 4
                    }

                    if (randNum == 0) {

                        int salvation = rand() % 3;  // 0, 1, 2

                        if (salvation > 1) {
                            continue;
                        }

                        int prevX = figuras[i].x;
                        int prevY = figuras[i].y;

                        // Eliminar la figura i
                        figuras[i] = figuras[N - 1];
                        #pragma omp critical
                        {
                            figuras = (Figura *) realloc(figuras, (N - 1) * sizeof(Figura));
                        }
                        N--;

                        // Crear una explosión en la posición de la figura eliminada
                        spawnExplosion(renderer, prevX, prevY);
                    } else if (randNum == 1) {

                        int salvation = rand() % 3;  // 0, 1, 2

                        if (salvation > 1) {
                            continue;
                        }

                        int prevX = figuras[j].x;
                        int prevY = figuras[j].y;

                        // Eliminar la figura j
                        figuras[j] = figuras[N - 1];
                        #pragma omp critical
                        {
                            figuras = (Figura *) realloc(figuras, (N - 1) * sizeof(Figura));
                        }
                        N--;

                        // Crear una explosión en la posición de la figura eliminada
                        spawnExplosion(renderer, prevX, prevY);
                    } else if (randNum == 2) {
                        continue;  // Ambas se salvan
                    } else if (randNum == 3) {

                        int children = rand() % 5;  // 0, 1, 2, 3, 4

                        for (int k = 0; k < children; k++) {
                            spawnFigura(renderer);
                        }

                    } else {
                        int prevX1 = figuras[i].x;
                        int prevY1 = figuras[i].y;
                        int prevX2 = figuras[j].x;
                        int prevY2 = figuras[j].y;

                        // Eliminar ambas figuras
                        figuras[i] = figuras[N - 1];
                        figuras[j] = figuras[N - 2];
                        #pragma omp critical
                        {
                            figuras = (Figura *) realloc(figuras, (N - 2) * sizeof(Figura));
                        }
                        N -= 2;

                        // Crear una explosión en la posición de la figura eliminada
                        spawnExplosion(renderer, prevX1, prevY1);
                        spawnExplosion(renderer, prevX2, prevY2);
                    }
                }
            }
        }

        #pragma omp barrier

        // Dibujar las figuras
        for (int i = 0; i < N; i++) {
            drawFigura(renderer, &figuras[i]);
        }

        // Dibujar explosiones
        for (int i = 0; i < E; i++) {
            drawFigura(renderer, &explosion[i]);
        }

        logTicks = SDL_GetTicks();  // Ticks antes del delay

        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / FPS);

        endTicks = SDL_GetTicks();
        fps = 1000.0f / (endTicks - startTicks);

        // Guardar en el log el tiempo que tardó en ejecutarse el ciclo y los FPS
        fprintf(file, "%d %d %.2f %d\n", logTicks-startTicks, endTicks-startTicks, fps, N);

        snprintf(title, sizeof(title), "Screen Saver - FPS: %.2f", fps);
        SDL_SetWindowTitle(window, title);
    }

    cleanup(window);

    return 0;
}

