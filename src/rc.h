#pragma once
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <fstream>
#include "color.h"
#include <cmath>

struct Player {
    float x;
    float y;
    float a;
    float fov;
};

const Color B = {0, 0, 0};
const Color W = {255, 255, 255};
const int blockSize = 50;
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;
const int scale = 100;

class Raycaster {
public:
    Raycaster(SDL_Renderer* renderer)
            : renderer(renderer) {

        player.x = blockSize + blockSize / 2;  // Posicionando el jugador en el centro de la pantalla
        player.y = blockSize + blockSize / 2;

        player.a = M_PI / 3.0f;
        player.fov = M_PI / 3.0f;
    }

    void circle(int x, int y, int radius, Color c) {
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        for (int w = 0; w < radius * 2; w++) {
            for (int h = 0; h < radius * 2; h++) {
                int dx = radius - w;  // Horizontal offset
                int dy = radius - h;  // Vertical offset
                if ((dx*dx + dy*dy) <= (radius * radius)) {
                    SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                }
            }
        }
    }


    void loadMap(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        while (std::getline(file, line)) {
            map.push_back(line);
        }
    }

    void point(int x, int y, Color c) {
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        SDL_RenderDrawPoint(renderer, x, y);
    }

    void rect(int x, int y, Color c) {
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        SDL_Rect rect = {x * blockSize, y * blockSize, blockSize, blockSize};
        SDL_RenderFillRect(renderer, &rect);
    }

    int cast_ray(float a) {
        int  c = 0;
        while(true){
            int x = static_cast<int>(player.x + c * cos(a));
            int y = static_cast<int>(player.y + c * sin(a));

            int i= static_cast<int>(x / blockSize);
            int j= static_cast<int>(y / blockSize);

            if(map[j][i] != ' '){
                point(x, y, W);
                break;
            }

            point(x, y, W);

            c += 10.0f;

        }
        return c;
    }

    void draw_Stake(int x,int h, Color c ){
        int start = (SCREEN_HEIGHT - h) / 2;
        int end = (SCREEN_WIDTH + h) / 2;
        SDL_Rect rect = {x, start, 1, end - start};
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);

        SDL_RenderFillRect(renderer, &rect);

    }

    void render() {
        for (int x = 0; x < SCREEN_WIDTH / blockSize; x++) {
            for (int y = 0; y < SCREEN_HEIGHT / blockSize; y++) {
                if (map[y][x] != ' ') {
                    rect(x, y, W);
                }
            }
        }

        for (int i = 0; i < SCREEN_WIDTH; i ++) {
            float a = player.a + player.fov / 2.0f + player.fov * i / static_cast<float>(SCREEN_WIDTH);
            cast_ray(a);
        }

        for (int i = 0; i < SCREEN_WIDTH; i ++) {
            float a = player.a + player.fov / 2.0f + player.fov * i / static_cast<float>(SCREEN_WIDTH);
            int d = cast_ray(a);
            int x = SCREEN_WIDTH + i;
            int h = static_cast<int>(SCREEN_HEIGHT / (d * cos(a - player.a)) * scale);
            draw_Stake(x, h, W);
        }

        draw_Stake(SCREEN_WIDTH+SCREEN_WIDTH/ 2, 500, W);

        circle(player.x, player.y, 5, W);  // Dibujando al jugador como un círculo de radio 5

    }




    Player player;
private:
    SDL_Renderer *renderer;
    std::vector<std::string> map;
};
