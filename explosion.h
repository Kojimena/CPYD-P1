#pragma once

#include "figure.h"

/**
 * @brief Estructura que representa una explosión en la pantalla.
 *
 * La explosión contiene una figura y un contador de frames.
 * Esta estructura se utiliza para manejar las explosiones en el juego o simulación,
 * permitiendo que cada explosión se renderice y se gestione durante un número específico de frames.
 */
typedef struct {
    Figura figura; ///< La figura que representa la explosión, incluyendo su posición y dimensiones.
    int frames;    ///< Número de frames restantes para que la explosión esté activa.
} Explosion;