#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <time.h>

#include "figure.h"
#include "explosion.h"

#include "omp.h"

#define FPS 120

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define FIGURE_WIDTH 32
#define FIGURE_HEIGHT 19

int N;  ///< Número de figuras.
Figura* figuras;  ///< Arreglo dinámico de figuras.

int E = 0;  ///< Número de explosiones activas.
Explosion *explosion;  ///< Arreglo dinámico de explosiones.
int EXPLOSION_FRAMES;  ///< Número de frames que dura una explosión.

/**
 * @brief Verifica si dos figuras se superponen.
 *
 * @param a Puntero a la primera figura.
 * @param b Puntero a la segunda figura.
 * @return int 1 si se superponen, 0 en caso contrario.
 */
int isOverlapping(Figura* a, Figura* b) {
    return (a->x < b->x + b->width && a->x + a->width > b->x &&
            a->y < b->y + b->height && a->y + a->height > b->y);
}

/**
 * @brief Inicializa las figuras, asegurándose de que no estén superpuestas.
 *
 * Esta función utiliza OpenMP para paralelizar la inicialización de las figuras,
 * distribuyendo las tareas de inicialización y verificación de superposición entre múltiples hilos.
 * Se asegura de que las figuras generadas no se superpongan entre sí.
 *
 * @param renderer El renderer de SDL utilizado para dibujar las figuras.
 */
void initFiguras(SDL_Renderer *renderer) {
    figuras = (Figura*)malloc(N * sizeof(Figura));  // Asignar memoria para N figuras
    omp_lock_t lock;
    omp_init_lock(&lock);
    #pragma omp parallel
    {
        #pragma omp for schedule(static)
        for (int i = 0; i < N; i++) {
            int x, y, spd_x, spd_y;
            SDL_Color color = {rand() % 256, rand() % 256, rand() % 256, 255};

            char *image = "assets/dvd_logo.png";
            Figura nuevaFigura;

            int overlapping;
            do {
                // Generar posición y velocidad aleatoria
                x = rand() % (SCREEN_WIDTH - 64);  // [0, SCREEN_WIDTH - figura_width]
                y = rand() % (SCREEN_HEIGHT - 38);  // [0, SCREEN_HEIGHT - figura_height]

                spd_x = rand() % 5 - 2;  // [-2, 2]
                spd_y = rand() % 5 - 2;  // [-2, 2]

                // Crear la nueva figura temporalmente
                nuevaFigura = createFigura(x, y, FIGURE_WIDTH, FIGURE_HEIGHT, spd_x, spd_y, image, renderer, color);

                // Verificar si se superpone con alguna figura existente
                overlapping = 0;
                #pragma omp critical
                {
                    for (int j = 0; j < i; j++) {
                        if (isOverlapping(&nuevaFigura, &figuras[j])) {
                            overlapping = 1;
                            break;
                        }
                    }
                }
            } while (overlapping);  // Repetir hasta que no haya superposición

            // Guardar la figura generada sin superposición
            figuras[i] = nuevaFigura;
        }
    }
}

/**
 * @brief Añade una nueva figura a la lista de figuras.
 *
 * Esta función utiliza OpenMP para paralelizar el proceso de generación y verificación
 * de colores de las figuras para asegurar que las nuevas figuras tengan características
 * variadas y no redundantes.
 *
 * @param renderer El renderer de SDL utilizado para dibujar las figuras.
 */
void spawnFigura(SDL_Renderer *renderer) {
    int x, y, spd_x, spd_y;
    SDL_Color color = {rand() % 256, rand() % 256, rand() % 256, 255};

    #pragma omp parallel for schedule(static) shared(figuras, color)
    for (int r = 255; r < 256; r += 16) {
        for (int g = 0; g < 256; g += 16) {
            for (int b = 255; b < 256; b += 16) {
                for (int i = 0; i < N; i++) {
                    if (figuras[i].color.r == r) {
                        color.r = rand() % 256;
                    }

                    if (figuras[i].color.g == g) {
                        color.g = rand() % 256;
                    }

                    if (figuras[i].color.b == b) {
                        color.b = rand() % 256;
                    }
                }
            }
        }
    }

    char *image = "assets/dvd_logo.png";
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
    figuras = (Figura*)realloc(figuras, (N + 1) * sizeof(Figura));

    // Guardar la nueva figura
    figuras[N] = nuevaFigura;

    // Incrementar el contador de figuras
    N++;
}

/**
 * @brief Crea una explosión en la posición especificada, que previamente era la posición de una figura.
 *
 * Esta función paraleliza la generación de explosiones, asegurando que cada explosión
 * tenga un color único generado aleatoriamente, sin interferencias de otros hilos.
 *
 * @param renderer El renderer de SDL utilizado para dibujar la explosión.
 * @param x Posición en el eje X donde ocurre la explosión.
 * @param y Posición en el eje Y donde ocurre la explosión.
 */
void spawnExplosion(SDL_Renderer *renderer, int x, int y) {
    int spd_x, spd_y;
    SDL_Color color = {rand() % 256, rand() % 256, rand() % 256, 255};

    omp_lock_t lock;
    omp_init_lock(&lock);

    #pragma omp parallel for schedule(static) shared(explosion, color)
    for (int r = 255; r < 256; r += 16) {
        for (int g = 0; g < 256; g += 16) {
            for (int b = 255; b < 256; b += 16) {
                for (int i = 0; i < E; i++) {
                    if (explosion[i].figura.color.r == r) {
                        color.r = rand() % 256;
                    }

                    if (explosion[i].figura.color.g == g) {
                        color.g = rand() % 256;
                    }

                    if (explosion[i].figura.color.b == b) {
                        color.b = rand() % 256;
                    }
                }
            }
        }
    }

    char *image = "assets/explosion.png";
    Explosion nuevaExplosion;

    // Las explosiones no se mueven
    spd_x = 0;
    spd_y = 0;

    nuevaExplosion.figura = createFigura(x, y, FIGURE_WIDTH * 4, FIGURE_HEIGHT * 4, spd_x, spd_y, image, renderer, color);
    nuevaExplosion.frames = EXPLOSION_FRAMES;

    explosion = (Explosion*)realloc(explosion, (E + 1) * sizeof(Explosion));
    explosion[E] = nuevaExplosion;
    #pragma omp atomic
    E++;
}

/**
 * @brief Limpia las explosiones que han finalizado y reduce el tamaño de las restantes.
 *
 * Esta función paraleliza el proceso de actualización y eliminación de explosiones,
 * asegurando que las operaciones de modificación de explosiones sean seguras en un entorno
 * paralelo utilizando OpenMP.
 *
 * @param explosion Arreglo de explosiones.
 */
void cleanExplosions() {
    #pragma omp parallel shared(explosion, E, EXPLOSION_FRAMES) default(none)
    {
        #pragma omp for schedule(dynamic)
        for (int i = 0; i < E; i++) {
            explosion[i].frames--;

            if (explosion[i].frames > 0) {
                explosion[i].figura.width -= (FIGURE_WIDTH * 4) / EXPLOSION_FRAMES;
                explosion[i].figura.height -= (FIGURE_HEIGHT * 4) / EXPLOSION_FRAMES;
            }
        }

        #pragma omp single
        {
            for (int i = 0; i < E;) {
                if (explosion[i].frames == 0) {
                    #pragma omp critical
                    {
                        explosion[i] = explosion[E - 1];
                        explosion = (Explosion*)realloc(explosion, (E - 1) * sizeof(Explosion));
                        E--;
                    }
                } else {
                    i++;  // Sólo avanzar el índice si no hemos eliminado un elemento.
                }
            }
        }
    }
}

/**
 * @brief Función para inicializar SDL y crear una ventana.
 *
 * @param title Título de la ventana.
 * @param width Ancho de la ventana.
 * @param height Alto de la ventana.
 * @return SDL_Window* Puntero a la ventana creada. Retorna NULL si ocurre un error.
 */
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

/**
 * @brief Función para cerrar SDL y destruir la ventana.
 *
 * @param window Puntero a la ventana de SDL que se va a destruir.
 */
void cleanup(SDL_Window* window) {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/**
 * @brief Resuelve la colisión entre dos figuras invirtiendo sus velocidades y ajustando sus posiciones.
 *
 * @param a Puntero a la primera figura involucrada en la colisión.
 * @param b Puntero a la segunda figura involucrada en la colisión.
 */
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

/**
 * @brief Actualiza las posiciones de las figuras y gestiona las colisiones con los bordes de la pantalla.
 *
 * Esta función utiliza OpenMP para paralelizar la actualización de las posiciones de las figuras,
 * permitiendo un manejo eficiente de múltiples figuras en movimiento.
 *
 * @param figuras Arreglo de figuras a actualizar.
 * @param N Número de figuras.
 * @param renderer Renderer de SDL utilizado para dibujar las figuras.
 */
void updateFiguras(Figura* figuras, int N, SDL_Renderer* renderer) {
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        Figura* figura = &figuras[i];

        figura->x += figura->speedX;
        figura->y += figura->speedY;

        // Cambiar dirección si toca los bordes de la pantalla
        if (figura->x <= 0 || figura->x >= (SCREEN_WIDTH - figura->width)) {
            figura->speedX *= -1;

            // Protección para evitar problemas en paralelo
            #pragma omp critical
            {
                spawnFigura(renderer);
            }
        }
        if (figura->y <= 0 || figura->y >= (SCREEN_HEIGHT - figura->height)) {
            figura->speedY *= -1;

            // Protección para evitar problemas en paralelo
            #pragma omp critical
            {
                spawnFigura(renderer);
            }
        }
    }
}

/**
 * @brief Función principal que ejecuta el salvapantallas.
 *
 * Este programa utiliza SDL para mostrar figuras en movimiento y gestionar colisiones.
 * Se paralelizan varias secciones del código para mejorar la eficiencia y el rendimiento.
 *
 * @param argc Número de argumentos de línea de comandos.
 * @param argv Arreglo de argumentos de línea de comandos.
 * @return int Código de salida del programa.
 */
int main(int argc, char *argv[]) {
    omp_set_nested(1);
    omp_set_dynamic(1);

    // usar una semilla
    srand(time(NULL));

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

    N = 5000;

    FILE *file = fopen("iter.log", "w");

    if (file == NULL) {
        printf("Error al abrir el archivo\n");
        return 1;
    }

    fprintf(file, "%d %d %d %d\n", N, EXPLOSION_FRAMES, SCREEN_WIDTH, SCREEN_HEIGHT);

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
        updateFiguras(figuras, N, renderer);

        // Detectar colisiones entre las figuras
        for (int i = 0; i < N; i++) {
            for (int j = i + 1; j < N; j++) {
                if (checkCollision(&figuras[i], &figuras[j])) {
                    resolveCollision(&figuras[i], &figuras[j]);

                    // decidir al azar cuál de las dos figuras se elimina
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
                        figuras = (Figura*)realloc(figuras, (N - 1) * sizeof(Figura));
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
                        figuras = (Figura*)realloc(figuras, (N - 1) * sizeof(Figura));
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
            drawFigura(renderer, &explosion[i].figura);
        }

        logTicks = SDL_GetTicks();  // Ticks antes del delay

        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / FPS);

        endTicks = SDL_GetTicks();
        fps = 1000.0f / (endTicks - startTicks);

        // Guardar en el log el tiempo que tardó en ejecutarse el ciclo y los FPS
        fprintf(file, "%d %d %.2f %d\n", logTicks - startTicks, endTicks - startTicks, fps, N);

        snprintf(title, sizeof(title), "Screen Saver - FPS: %.2f", fps);
        SDL_SetWindowTitle(window, title);
    }

    cleanup(window);

    return 0;
}
