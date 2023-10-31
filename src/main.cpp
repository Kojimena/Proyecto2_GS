#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <print.h>
#include <sstream>
#include "color.h"
#include "imageloader.h"
#include "rc.h"

SDL_Window* window;
SDL_Renderer* renderer;
const int FRAME_DELAY = 1000 / 40;  // 1000 ms (1 second) divided by 30 (for 30 FPS)
Uint32 frameStart, frameTime;

void clear() {
    SDL_SetRenderDrawColor(renderer, 56, 56, 56, 255);
    SDL_RenderClear(renderer);
}

void draw_floor() {
    // floor color
    SDL_SetRenderDrawColor(renderer, 0, 120, 0, 0);
    SDL_Rect rect = {
            0,                        // x start point
            SCREEN_HEIGHT / 2,        // y start point
            SCREEN_WIDTH,             // full width
            SCREEN_HEIGHT          // half height
    };
    SDL_RenderFillRect(renderer, &rect);
}

int main() {
    print("hello world");

    SDL_Init(SDL_INIT_VIDEO);
    ImageLoader::init();

    window = SDL_CreateWindow("DOOM", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT + MINIMAP_WIDTH *1.5, SDL_WINDOW_SHOWN);
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
        frameStart = SDL_GetTicks();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        r.player.a += 3.14 / 24;
                        break;
                    case SDLK_RIGHT:
                        r.player.a -= 3.14 / 24;
                        break;
                    case SDLK_UP:
                        r.movePlayer(speed * cos(r.player.a), speed * sin(r.player.a));
                        break;
                    case SDLK_DOWN:
                        r.movePlayer(-speed * cos(r.player.a), -speed * sin(r.player.a));
                        break;
                    default:
                        break;
                }
            }


            frameTime = SDL_GetTicks() - frameStart;

            if (FRAME_DELAY > frameTime) {
                SDL_Delay(FRAME_DELAY - frameTime);
            }

            // Calculate frames per second and update window title
            std::ostringstream titleStream;
            titleStream << "FPS: " << 1000.0 / (SDL_GetTicks() - frameStart);  // Milliseconds to seconds
            SDL_SetWindowTitle(window, titleStream.str().c_str());
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
