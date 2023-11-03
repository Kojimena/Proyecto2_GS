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
SDL_AudioSpec wavSpec;
Uint32 wavLength;
Uint8 *wavBuffer;
SDL_AudioDeviceID deviceId;

SDL_AudioSpec effectSpec;
Uint32 effectLength;
Uint8 *effectBuffer;
SDL_AudioDeviceID effectDevice;


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

    // Inicializar SDL con soporte para video y audio
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Init(SDL_INIT_VIDEO);
    ImageLoader::init();

    window = SDL_CreateWindow("DOOM", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT + MINIMAP_WIDTH *1.5, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    ImageLoader::loadImage("+", "../assets/wall3.png");
    ImageLoader::loadImage("-", "../assets/wall1.png");
    ImageLoader::loadImage("|", "../assets/wall2.png");
    ImageLoader::loadImage("*", "../assets/wall4.png");
    ImageLoader::loadImage("g", "../assets/wall5.png");
    ImageLoader::loadImage("player", "../assets/hand.png");
    ImageLoader::loadImage("player_alt", "../assets/hand_alt.png");


    Raycaster r = { renderer };
    r.load_map("../assets/map.txt");

    // Cargar y reproducir el archivo WAV
    if (SDL_LoadWAV("../assets/first.wav", &wavSpec, &wavBuffer, &wavLength) == NULL) {
        fprintf(stderr, "Could not open test.wav: %s\n", SDL_GetError());
    } else {
        deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
        if (deviceId == 0) {
            fprintf(stderr, "Could not open audio: %s\n", SDL_GetError());
        } else {
            SDL_QueueAudio(deviceId, wavBuffer, wavLength);
            SDL_PauseAudioDevice(deviceId, 0);
        }
    }

    // Cargar el efecto de sonido
    if (SDL_LoadWAV("../assets/fireball.wav", &effectSpec, &effectBuffer, &effectLength) == NULL) {
        fprintf(stderr, "Could not open switch.wav: %s\n", SDL_GetError());
    } else {
        effectDevice = SDL_OpenAudioDevice(NULL, 0, &effectSpec, NULL, 0);
        if (effectDevice == 0) {
            fprintf(stderr, "Could not open audio for effect: %s\n", SDL_GetError());
        }
    }

    // Cargar el efecto de sonido de la meta
    if (SDL_LoadWAV("../assets/goal.wav", &goalSpec, &goalBuffer, &goalLength) == NULL) {
        fprintf(stderr, "Could not open goal.wav: %s\n", SDL_GetError());
    } else {
        goalDevice = SDL_OpenAudioDevice(NULL, 0, &goalSpec, NULL, 0);
        if (goalDevice == 0) {
            fprintf(stderr, "Could not open audio for goal effect: %s\n", SDL_GetError());
        }
    }


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
                    case SDLK_SPACE:
                        r.switchSprite();
                        // Reproducir el efecto de sonido
                        if (effectDevice != 0) {
                            SDL_ClearQueuedAudio(effectDevice);
                            SDL_QueueAudio(effectDevice, effectBuffer, effectLength);
                            SDL_PauseAudioDevice(effectDevice, 0);
                        }
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
        r.drawPlayerSprite();


        SDL_RenderPresent(renderer);

    }

    // Limpieza del audio
    if (deviceId != 0) {
        SDL_CloseAudioDevice(deviceId);
    }
    if (wavBuffer != NULL) {
        SDL_FreeWAV(wavBuffer);
    }

    if (effectDevice != 0) {
        SDL_CloseAudioDevice(effectDevice);
    }
    if (effectBuffer != NULL) {
        SDL_FreeWAV(effectBuffer);
    }


    // Limpieza del audio de la meta
    if (goalDevice != 0) {
        SDL_CloseAudioDevice(goalDevice);
    }
    if (goalBuffer != NULL) {
        SDL_FreeWAV(goalBuffer);
    }

    // Limpieza de SDL
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

    SDL_DestroyWindow(window);
    SDL_Quit();
}
