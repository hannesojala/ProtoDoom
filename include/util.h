#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <vector>

// Rudimentary 2d vector
struct vec2 {
    float x, y;
    vec2() : x(0), y(0) {}
    vec2(float xx, float yy) : x(xx), y(yy) {}
    vec2 operator+(const vec2& rhs) {
        return vec2(x + rhs.x, y + rhs.y);
    }
    vec2 operator-(const vec2& rhs) {
        return vec2(x - rhs.x, y - rhs.y);
    }
    vec2 operator*(const float& scalar) {
        return vec2(x*scalar, y*scalar);
    }
};

struct Ray {
    vec2 o;
    vec2 d;
};

struct RGBA {
    int r, g, b, a;
};

// Wrapper for SDL_Window/Renderer pair
class Window {
private:
    SDL_Window* pWindow;
    SDL_Renderer* pRenderer;
public:
    Window(std::string title, int w, int h) {
        pWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
        pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);
        // Transparency
        SDL_SetRenderDrawBlendMode(pRenderer, SDL_BLENDMODE_BLEND);
    }
    Window(std::string title, int w, int h, int px, int py) {
        pWindow = SDL_CreateWindow(title.c_str(), px, py, w, h, SDL_WINDOW_SHOWN);
        pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);
        // Transparency
        SDL_SetRenderDrawBlendMode(pRenderer, SDL_BLENDMODE_BLEND);
    }
    ~Window() {
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
    }
    void setTitle(std::string title) {
        SDL_SetWindowTitle(pWindow, title.c_str());
    }
    void getPos(int* x, int* y) {
        SDL_GetWindowPosition(pWindow, x, y);
    }
    // Todo: make better drawing (option: dont set color every time)
    void drawLine(int x1, int y1, int x2, int y2, RGBA clr) {
        SDL_SetRenderDrawColor(pRenderer, clr.r, clr.g, clr.b, clr.a);
        SDL_RenderDrawLine(pRenderer, x1, y1, x2, y2);
    }
    void drawRect(SDL_Rect rect, RGBA clr, bool filled) {
        SDL_SetRenderDrawColor(pRenderer, clr.r, clr.g, clr.b, clr.a);
        if (filled) SDL_RenderFillRect(pRenderer, &rect);
        else SDL_RenderDrawRect(pRenderer, &rect);
    }
    void clear(RGBA clr) {
        SDL_SetRenderDrawColor(pRenderer, clr.r, clr.g, clr.b, clr.a);
        SDL_RenderClear(pRenderer);
    }
    void render() {
        SDL_RenderPresent(pRenderer);
    }
    void focus() {
        // SDL_SetWindowInputFocus(pWindow); // does not raise AND focus
        SDL_RaiseWindow(pWindow);
    }
};

// Generation!
/*
float floorOffset = 1;
float cielOffset = 1;
Wall door(vec2{-2,0}, vec2{2,0}, floorOffset, cielOffset);
for (int i = 0; i < 500; i++) {
    vec2 exitOffset = {float(rand()%16 - 8), float(rand()%6 + 2)};
    Wall exit(door.p1 + exitOffset, door.p2 + exitOffset, floorOffset, cielOffset);

    Wall L(door.p1, exit.p1, floorOffset, cielOffset);
    Wall R(door.p2, exit.p2, floorOffset, cielOffset);

    walls.push_back(L);
    walls.push_back(R);

    door = exit;

    floorOffset += 1.0f;
    cielOffset  += 1.0f;
}
*/
/*std::vector<vec2> points = {vec2(0,0)};
for (int i = 1; i < 256; i++) {
    vec2 point = vec2(float(rand()%32 - 16), float(rand()%32 - 16));
    points.push_back(point);
}

for (vec2 point : points) {
    vec2 nearest;
    float dist = INFINITY;
    for (vec2 otherPoint : points) {
        if (otherPoint.x != point.x && otherPoint.y != point.y) {
            float d = sqrt((point.x-otherPoint.x)*(point.x-otherPoint.x) + (point.y-otherPoint.y)*(point.y-otherPoint.y));
            if (d < dist) {
                nearest = vec2{otherPoint.x, otherPoint.y};
                dist = d;
            }
        }
    }
    std::cout << dist << std::endl;
    walls.push_back(Wall(point, nearest, float(rand()%4-2), float(rand()%4-2)));
}*/

