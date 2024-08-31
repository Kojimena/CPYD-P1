# CPYD-P1
Computación Paralela y distribuida - Proyecto 1

# Screen Saver Simulation

Este proyecto es una simulación de un salvapantallas que muestra múltiples figuras en movimiento que rebotan en los bordes de la pantalla. Las figuras pueden colisionar entre sí, lo que provoca una explosión visual en la pantalla.

## Características

- **Movimiento de figuras:** Las figuras se mueven en diferentes direcciones con velocidades aleatorias.
- **Colisiones:** Cuando dos figuras colisionan, se resuelven las colisiones invirtiendo sus direcciones y creando explosiones.
- **Explosiones:** Las explosiones se crean en la posición de las figuras eliminadas y se reducen gradualmente hasta desaparecer.
- **Control de FPS:** El programa controla la tasa de fotogramas por segundo para asegurar un rendimiento fluido.

## Requisitos

- **SDL2:** Biblioteca para crear gráficos y manejar eventos.
- **SDL2_image:** Biblioteca adicional para cargar imágenes en diferentes formatos.
- **C Compiler:** Se requiere un compilador compatible con C (como GCC o Clang) para compilar el código.

## Instalación

1. **Instalar SDL2 y SDL2_image:**

   En sistemas basados en Debian/Ubuntu:
   \`\`\`bash
   sudo apt-get install libsdl2-dev libsdl2-image-dev
   \`\`\`

   En sistemas basados en Fedora:
   \`\`\`bash
   sudo dnf install SDL2-devel SDL2_image-devel
   \`\`\`

   En sistemas macOS:
   \`\`\`bash
   brew install sdl2 sdl2_image
   \`\`\`

2. **Clonar el repositorio:**
   \`\`\`bash
   git clone https://github.com/Kojimena/CPYD-P1.git
   cd CPYD-P1
   \`\`\`

3. **Compilar el código:**
   \`\`\`bash
   gcc -o screen_saver main.c figure.c explosion.c -lSDL2 -lSDL2_image -lm
   \`\`\`

## Uso

Para ejecutar la simulación, simplemente ejecuta el archivo compilado:

\`\`\`bash
./screen_saver <N> <E>
\`\`\`

Donde:
- `N` es el número inicial de figuras en pantalla.
- `E` es el número de frames que durará cada explosión.

Ejemplo:

\`\`\`bash
./screen_saver 5000 30
\`\`\`

## Estructura del Proyecto

- **main.c:** Contiene la función principal y gestiona el bucle de simulación.
- **figure.c:** Contiene funciones para manejar las figuras, incluyendo su creación, movimiento y detección de colisiones.
- **explosion.c:** Maneja las explosiones generadas por las colisiones de figuras.
- **figure.h y explosion.h:** Archivos de cabecera que definen las estructuras y funciones utilizadas en el proyecto.

## Personalización

Puedes modificar el tamaño de la pantalla y las dimensiones de las figuras en los archivos `figure.h` y `main.c` cambiando los valores de `SCREEN_WIDTH`, `SCREEN_HEIGHT`, `FIGURE_WIDTH`, y `FIGURE_HEIGHT`.

## Contribuciones

Las contribuciones son bienvenidas. Si encuentras un error o tienes una idea para mejorar el proyecto, por favor, abre un "issue" o envía un "pull request".

## Licencia

Este proyecto está bajo la Licencia MIT. Consulta el archivo `LICENSE` para más detalles.

## Créditos

Este proyecto fue desarrollado por [Mark](https://github.com/markalbrand56), [Jime](https://github.com/Kojimena) y [Javier](https://github.com/XJ4v1erX).
