# Raycasting

Ray Caster que renderiza un nivel entero y jugable basado en super mario bros.

## Demostración

Se puede ver el juego en acción en este [video]().

## Puntos cumplidos

- (30 puntos) [Criterio subjetivo] Según la estética de su nivel
- (10 puntos) por mantener al menos 30 fps constantemente (los fps se deben desplegar)
- (20 puntos) Por implementar una cámara con movimiento hacia delante y hacia atrás y rotación
- (10 puntos) Por implementar un minimapa que muestre la posición de jugador en el mundo. No puede estar lado a lado del mapa principal, debe estar en una esquina. 
- (10 puntos) Por agregar efectos de sonido
- (20 puntos) Por agregar al menos 1 animación a alguna sprite en la pantalla
- (5 puntos) Por agregar música de fondo.
- 10 puntos más por implementar rotación con el mouse (solo horizontal)
- (5 puntos) Por agregar una pantalla de bienvenida

## Características

- **Navegación**: Los jugadores pueden moverse por el mapa, que se carga desde un archivo de texto.
- **Sistema de colisiones**: El juego detecta colisiones con paredes y otros obstáculos en el mapa.
- **Control de teclado y mouse**: Los jugadores pueden usar el teclado para moverse y el mouse para girar.
- **Efectos de sonido**: Incluye efectos de sonido para las acciones del jugador y el ambiente del juego.
- **Minimapa**: Se muestra un minimapa para ayudar en la navegación.
- **Raycasting**: Implementa un sistema de raycasting básico para renderizar las vistas en primera persona.


## Instalación

Para compilar y ejecutar el juego, se necesita tener SDL2, SDL2_image y SDL2_mixer instalados en el sistema.

```bash
gcc main.c -o game -lSDL2 -lSDL2_image -lSDL2_mixer
./gam
```