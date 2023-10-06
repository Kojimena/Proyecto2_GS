#pragma once
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>

struct Color {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;

    Color() : r(0), g(0), b(0), a(255) {}

    Color(int red, int green, int blue, int alpha = 255) {
        r = static_cast<Uint8>(std::min(std::max(red, 0), 255));
        g = static_cast<Uint8>(std::min(std::max(green, 0), 255));
        b = static_cast<Uint8>(std::min(std::max(blue, 0), 255));
        a = static_cast<Uint8>(std::min(std::max(alpha, 0), 255));
    }

    Color(float red, float green, float blue, float alpha = 1.0f) {
        r = static_cast<Uint8>(std::min(std::max(red * 255, 0.0f), 255.0f));
        g = static_cast<Uint8>(std::min(std::max(green * 255, 0.0f), 255.0f));
        b = static_cast<Uint8>(std::min(std::max(blue * 255, 0.0f), 255.0f));
        a = static_cast<Uint8>(std::min(std::max(alpha * 255, 0.0f), 255.0f));
    }

    // Overload the + operator to add colors
    Color operator+(const Color& other) const {
        return Color(
            std::min(255, int(r) + int(other.r)),
            std::min(255, int(g) + int(other.g)),
            std::min(255, int(b) + int(other.b)),
            std::min(255, int(a) + int(other.a))
        );
    }

    // Overload the * operator to scale colors by a factor
    Color operator*(float factor) const {
        return Color(
            std::clamp(static_cast<Uint8>(r * factor), Uint8(0), Uint8(255)),
            std::clamp(static_cast<Uint8>(g * factor), Uint8(0), Uint8(255)),
            std::clamp(static_cast<Uint8>(b * factor), Uint8(0), Uint8(255)),
            std::clamp(static_cast<Uint8>(a * factor), Uint8(0), Uint8(255))
        );
    }

    // Friend function to allow float * Color
    friend Color operator*(float factor, const Color& color);
};