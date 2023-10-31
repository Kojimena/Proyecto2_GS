#pragma once

#include <print.h>
#include <iostream>
#include <fstream>
#include <SDL_render.h>
#include <string>
#include <vector>
#include <cmath>
#include <SDL2/SDL.h>
#include <unordered_map>
#include "color.h"
#include "imageloader.h"


const Color B = {0, 0, 0};
const Color W = {255, 255, 255};

const int WIDTH = 16;
const int HEIGHT = 11;
const int BLOCK = 50;
const int SCREEN_WIDTH = WIDTH * BLOCK;
const int SCREEN_HEIGHT = HEIGHT * BLOCK;

//mini mapa
const int MINIMAP_SCALE = 6;  // Escala del minimapa en relación con el tamaño del bloque
const int MINIMAP_WIDTH = WIDTH * BLOCK / MINIMAP_SCALE;


struct Player {
    int x;
    int y;
    float a;
    float fov;
};

struct Impact {
    float d;
    std::string mapHit;  // + | -
    int tx;
};

class Raycaster {
public:
    Raycaster(SDL_Renderer* renderer)
            : renderer(renderer) {

        player.x = BLOCK + BLOCK / 2;
        player.y = BLOCK + BLOCK / 2;

        player.a = M_PI / 4.0f;
        player.fov = M_PI / 3.0f;

        scale = 50;
        tsize = 128;
    }

    void load_map(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        while (getline(file, line)) {
            map.push_back(line);
        }
        file.close();
    }

    void point(int x, int y, Color c) {
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        SDL_RenderDrawPoint(renderer, x, y);
    }

    bool isWallCollision(int newX, int newY) {
        int i = newX / BLOCK;
        int j = newY / BLOCK;
        return map[j][i] != ' ';
    }

    void movePlayer(float deltaX, float deltaY) {
        int newX = player.x + deltaX;
        int newY = player.y + deltaY;

        if (!isWallCollision(newX, player.y)) {
            player.x = newX;
        }

        if (!isWallCollision(player.x, newY)) {
            player.y = newY;
        }
    }

    void rect(int x, int y, const std::string& mapHit, int scale = 1) {
        for(int cx = x; cx < x + BLOCK / scale; cx++) {
            for(int cy = y; cy < y + BLOCK / scale; cy++) {
                int tx = ((cx - x) * tsize * scale) / BLOCK;
                int ty = ((cy - y) * tsize * scale) / BLOCK;

                Color c = ImageLoader::getPixelColor(mapHit, tx, ty);
                SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b , 255);
                SDL_RenderDrawPoint(renderer, cx, cy);
            }
        }
    }

    Impact cast_ray(float a) {
        float d = 0;
        std::string mapHit;
        int tx;

        while(true) {
            int x = static_cast<int>(player.x + d * cos(a));
            int y = static_cast<int>(player.y + d * sin(a));

            int i = static_cast<int>(x / BLOCK);
            int j = static_cast<int>(y / BLOCK);


            if (map[j][i] != ' ') {
                mapHit = map[j][i];

                int hitx = x - i * BLOCK;
                int hity = y - j * BLOCK;
                int maxhit;

                if (hitx == 0 || hitx == BLOCK - 1) {
                    maxhit = hity;
                } else {
                    maxhit = hitx;
                }

                tx = maxhit * tsize / BLOCK;

                break;
            }

            //point(x, y, W);

            d += 1;
        }
        return Impact{d, mapHit, tx};
    }

    void draw_stake(int x, float h, Impact i) {
        float start = SCREEN_HEIGHT/2.0f - h/2.0f;
        float end = start + h;

        for (int y = start; y < end; y++) {
            int ty = (y - start) * tsize / h;
            Color c = ImageLoader::getPixelColor(i.mapHit, i.tx, ty);
            SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);

            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

    void render() {
        for (int x = 0; x < WIDTH * BLOCK; x += BLOCK) {
            for (int y = 0; y < HEIGHT * BLOCK; y += BLOCK) {
                int i = static_cast<int>(x / BLOCK);
                int j = static_cast<int>(y / BLOCK);

                if (map[j][i] != ' ') {
                    std::string mapHit(1, map[j][i]);

                    // Ajusta las coordenadas x y y para mover el minimapa a la esquina inferior derecha
                    int drawX = SCREEN_WIDTH - MINIMAP_WIDTH + x / MINIMAP_SCALE;
                    int drawY = SCREEN_HEIGHT*1.4 - MINIMAP_WIDTH + y / MINIMAP_SCALE;

                    rect(drawX, drawY, mapHit, MINIMAP_SCALE);
                }
            }
        }

        // Represent the player in the minimap
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect rect = {
                SCREEN_WIDTH - MINIMAP_WIDTH + player.x / MINIMAP_SCALE - 2,
                static_cast<int>(SCREEN_HEIGHT*1.4 - MINIMAP_WIDTH + player.y / MINIMAP_SCALE - 2),
                4,
                4
        };
        SDL_RenderFillRect(renderer, &rect);


        // draw the main screen (first-person view)
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            double a = player.a + player.fov / 2.0 - player.fov * i / SCREEN_WIDTH;
            Impact impact = cast_ray(a);
            float d = impact.d;
            int x = i;
            float h = static_cast<float>(SCREEN_HEIGHT)/static_cast<float>(d) * static_cast<float>(scale);
            draw_stake(x, h, impact);
        }
    }

    Player player;
private:
    int scale;
    SDL_Renderer* renderer;
    std::vector<std::string> map;
    int tsize;
};
