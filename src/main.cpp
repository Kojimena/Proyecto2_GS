#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <print.h>

#include "color.h"
#include "imageloader.h"
#include "rc.h"

SDL_Window* window;
SDL_Renderer* renderer;

void clear() {
    SDL_SetRenderDrawColor(renderer, 56, 56, 56, 255);
    SDL_RenderClear(renderer);
}

void draw_floor() {
    // floor color
    SDL_SetRenderDrawColor(renderer, 112, 122, 122, 255);
    SDL_Rect rect = {
            SCREEN_WIDTH,
            SCREEN_HEIGHT / 2,
            SCREEN_WIDTH,
            SCREEN_HEIGHT / 2
    };
    SDL_RenderFillRect(renderer, &rect);
}

int main() {
    print("hello world");

    SDL_Init(SDL_INIT_VIDEO);
    ImageLoader::init();

    window = SDL_CreateWindow("DOOM", 0, 0, SCREEN_WIDTH * 2, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    ImageLoader::loadImage("+", "../assets/wall3.png");
    ImageLoader::loadImage("-", "../assets/wall1.png");
    ImageLoader::loadImage("|", "../assets/wall2.png");
    ImageLoader::loadImage("*", "../assets/wall4.png");
    ImageLoader::loadImage("g", "../assets/wall5.png");

    Raycaster r = { renderer };
    r.load_map("../assets/map.txt");

    bool running = true;
    int speed = 10;
    while(running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
            if (event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym ){
                    case SDLK_LEFT:
                        r.player.a += 3.14/24;
                        break;
                    case SDLK_RIGHT:
                        r.player.a -= 3.14/24;
                        break;
                    case SDLK_UP:
                        r.player.x += speed * cos(r.player.a);
                        r.player.y += speed * sin(r.player.a);
                        break;
                    case SDLK_DOWN:
                        r.player.x -= speed * cos(r.player.a);
                        r.player.y -= speed * sin(r.player.a);
                        break;
                    default:
                        break;
                }
            }
        }

        clear();
        draw_floor();

        r.render();

        // render

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}
