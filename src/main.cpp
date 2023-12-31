#include <SDL2/SDL.h>
#include <print.h>
#include <sstream>
#include "imageloader.h"
#include "rc.h"
#include "SDL_ttf.h"

SDL_Window* window;
SDL_Renderer* renderer;
const int FRAME_DELAY = 1000/25;
Uint32 frameStart, frameTime;
SDL_AudioSpec wavSpec;
Uint32 wavLength;
Uint8 *wavBuffer;
SDL_AudioDeviceID deviceId;

SDL_AudioSpec effectSpec;
Uint32 effectLength;
Uint8 *effectBuffer;
SDL_AudioDeviceID effectDevice;


SDL_AudioSpec goalSpec;
Uint32 goalLength;
Uint8 *goalBuffer;
SDL_AudioDeviceID goalDevice;


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
    TTF_Init();
    ImageLoader::init();


    window = SDL_CreateWindow("DOOM", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT + MINIMAP_WIDTH *1.5, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Inicializar SDL_ttf
    TTF_Font * font = TTF_OpenFont("../assets/font.ttf", 25);
    if (!font) {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
        // Clean up SDL and exit
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_Color color = { 255, 255, 255 };
    SDL_Surface * surface = TTF_RenderText_Solid(font,
                                                 "Welcome to komario", color);
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

    int texW = 0;
    int texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = { 200, 0, texW, texH };

    // Imagen de bienvenida
    SDL_Texture* welcomeImage = IMG_LoadTexture(renderer, "../assets/welcome.png");
    if (!welcomeImage) {
        fprintf(stderr, "Failed to load welcome image: %s\n", IMG_GetError());
        // Limpieza en caso de que la imagen no se pueda cargar
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, welcomeImage, NULL, NULL);  // Renderizar en toda la pantalla
    SDL_RenderPresent(renderer);

    // Liberaración de textura
    SDL_DestroyTexture(welcomeImage);



    ImageLoader::loadImage("+", "../assets/wall3.png");
    ImageLoader::loadImage("-", "../assets/wall1.png");
    ImageLoader::loadImage("|", "../assets/wall2.png");
    ImageLoader::loadImage("*", "../assets/wall4.png");
    ImageLoader::loadImage("g", "../assets/wall5.png");
    ImageLoader::loadImage("player", "../assets/hand.png");
    ImageLoader::loadImage("player_alt", "../assets/hand_alt.png");


    Raycaster r = { renderer };
    r.load_map("../assets/map.txt");

    //Cargas de sonido
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

    if (SDL_LoadWAV("../assets/fireball.wav", &effectSpec, &effectBuffer, &effectLength) == NULL) {
        fprintf(stderr, "Could not open switch.wav: %s\n", SDL_GetError());
    } else {
        effectDevice = SDL_OpenAudioDevice(NULL, 0, &effectSpec, NULL, 0);
        if (effectDevice == 0) {
            fprintf(stderr, "Could not open audio for effect: %s\n", SDL_GetError());
        }
    }

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
    while(running && !r.player.gameWon) {
        SDL_Event event;
        frameStart = SDL_GetTicks();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
            if (event.type == SDL_MOUSEMOTION) {
                const float sensitivity = -0.005f;
                r.player.a += event.motion.xrel * sensitivity;
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

        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_RenderPresent(renderer);

    }

    if (r.player.gameWon) {
        // Renderizar mensaje de victoria
        if (goalDevice != 0) {
            SDL_ClearQueuedAudio(goalDevice);
            SDL_QueueAudio(goalDevice, goalBuffer, goalLength);
            SDL_PauseAudioDevice(goalDevice, 0);
        }

        //quitamos el audio de fondo
        if (deviceId != 0) {
            SDL_ClearQueuedAudio(deviceId);
            SDL_PauseAudioDevice(deviceId, 1);
            SDL_CloseAudioDevice(deviceId);

        }

        SDL_Color winColor = {255, 255, 0}; // Amarillo
        SDL_Surface* winSurface = TTF_RenderText_Solid(font, "You Win!", winColor);
        SDL_Texture* winTexture = SDL_CreateTextureFromSurface(renderer, winSurface);
        int winTexW = 0;
        int winTexH = 0;
        SDL_QueryTexture(winTexture, NULL, NULL, &winTexW, &winTexH);
        SDL_Rect winRect = {SCREEN_WIDTH/2 - winTexW/2, SCREEN_HEIGHT/2 - winTexH/2, winTexW, winTexH};

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, winTexture, NULL, &winRect);
        SDL_RenderPresent(renderer);

        // Pausar para que el mensaje sea visible
        SDL_Delay(4000); // Espera 5 segundos

        // Limpieza de recursos del mensaje de victoria
        SDL_DestroyTexture(winTexture);
        SDL_FreeSurface(winSurface);
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

    if (goalDevice != 0) {
        SDL_CloseAudioDevice(goalDevice);
    }
    if (goalBuffer != NULL) {
        SDL_FreeWAV(goalBuffer);
    }

    // Limpieza de SDL
    SDL_DestroyWindow(window);

    // Cleanup at the end
    TTF_CloseFont(font);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    IMG_Quit();

    return 0;
}
