#include <SDL2/SDL.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <iostream>
#include "color.h"
#include "rc.h"  // Asumo que rc.h es donde definiste la clase Raycaster

SDL_Window* window;
SDL_Renderer* renderer;

void clear() {
    SDL_SetRenderDrawColor(renderer, B.r, B.g, B.b, B.a);
    SDL_RenderClear(renderer);
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("DOOM", 0, 0, 2000, 1000, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Crear una instancia de Raycaster y cargar el mapa
    Raycaster raycaster(renderer);
    raycaster.loadMap("/Users/jime/Uvgcoding/graphics/Proyecto2_GS/src/map.txt"); // Reemplaza con la ruta a tu archivo de mapa

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                    break;
                }
                if (event.key.keysym.sym == SDLK_DOWN) {
                    raycaster.player.x += cos(raycaster.player.a) * 5;
                    raycaster.player.y += sin(raycaster.player.a) * 5;
                }
                if (event.key.keysym.sym == SDLK_UP) {
                    raycaster.player.x -= cos(raycaster.player.a) * 5;
                    raycaster.player.y -= sin(raycaster.player.a) * 5;
                }
                if (event.key.keysym.sym == SDLK_a) {
                    raycaster.player.x += cos(raycaster.player.a - M_PI / 2) * 5;
                    raycaster.player.y += sin(raycaster.player.a - M_PI / 2) * 5;
                }
                if (event.key.keysym.sym == SDLK_d) {
                    raycaster.player.x += cos(raycaster.player.a + M_PI / 2) * 5;
                    raycaster.player.y += sin(raycaster.player.a + M_PI / 2) * 5;
                }
                if (event.key.keysym.sym == SDLK_LEFT) {
                    raycaster.player.a -= 0.1;
                }
                if (event.key.keysym.sym == SDLK_RIGHT) {
                    raycaster.player.a += 0.1;
                }
            }
        }

        clear();

        // Renderizar el raycaster
        raycaster.render();

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}
