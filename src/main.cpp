#include "util.h"
#include "MapStructs.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>

// Todo - CLEAN THIS SHIT

// Features
// ....Collisions
// Rendering
// ....Texture mapping
// ....Sprites
// ....BSP tree

const int   WIDTH             = 720;
const int   HEIGHT            = 480;
const int   MAP_WIDTH_HEIGHT  = HEIGHT;
const float MOUSE_SENSITIVITY = 0.001;
const float PLAYERSPEED       = 5;

float FOV     = 90 * 3.1415/180.0f;
float MAPZOOM = 16.0f;

void drawWall(Window& window, int col, Line line, int whichSide, float dist, float playerZ)
{
    Sector *side1, *side2;
    if (whichSide == 1)
    {
        side1 = line.side1Sector;
        side2 = line.side2Sector;
    }
    else if (whichSide == 2)
    {
        side1 = line.side2Sector;
        side2 = line.side1Sector;
    }

    // Draw Full Wall
    if (line.isFullWall) {
        int top = HEIGHT/2 - (HEIGHT/dist * (side1->ceilHeight - playerZ)) / (FOV); // Divide by FOV for vertical adjustment
        int bot = HEIGHT/2 + (HEIGHT/dist * (side1->floorHeight + playerZ)) / (FOV); // change to add units
        window.drawLine(col, top, col, bot,
                        RGBA {int(100/std::max(dist+1.0f, 1.0f)), int(255/std::max(dist+1.0f, 1.0f)), int(255/std::max(dist+1.0f, 1.0f)), 255} );

        //ceiling above wall
        if (top > 0) window.drawLine(col, 0, col, top, side1->ceilColor);
        //floor below wall
        if (bot < HEIGHT) window.drawLine(col, HEIGHT, col, bot, side1->floorColor);
    }

    // Draw Top and Bottom wall
    else {
        int topTop = HEIGHT/2 - (HEIGHT/dist * (side1->ceilHeight - playerZ)) / (FOV); // Divide by FOV for vertical adjustment
        int botTop = HEIGHT/2 - (HEIGHT/dist * (side2->ceilHeight - playerZ)) / (FOV);
        window.drawLine(col, topTop, col, botTop,

                        RGBA {int(255/std::max(dist+1.0f, 1.0f)), int(255/std::max(dist+1.0f, 1.0f)), int(255/std::max(dist+1.0f, 1.0f)), 255} );
        int botBot = HEIGHT/2 + (HEIGHT/dist * (side1->floorHeight + playerZ)) / (FOV); // Divide by FOV for vertical adjustment
        int topBot = HEIGHT/2 + (HEIGHT/dist * (side2->floorHeight + playerZ)) / (FOV);
        window.drawLine(col, topBot, col, botBot,
                        RGBA {int(255/std::max(dist+1.0f, 1.0f)), int(255/std::max(dist+1.0f, 1.0f)), int(255/std::max(dist+1.0f, 1.0f)), 255} );

        //ceiling above top wall
        if (topTop > 0) window.drawLine(col, 0, col, topTop, side1->ceilColor );
        //floor below bottom wall
        if (botBot < HEIGHT) window.drawLine(col, botBot, col, HEIGHT, side1->floorColor );
    }
}

int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    Window mainWindow("Caster!", WIDTH, HEIGHT);
    int x;
    mainWindow.getPos(&x, nullptr);
    Window mapWindow("Caster Map", MAP_WIDTH_HEIGHT, MAP_WIDTH_HEIGHT, x - MAP_WIDTH_HEIGHT, SDL_WINDOWPOS_CENTERED);
    mainWindow.focus();

    vec2 playerPos(0,0);
    float playerZ = 0;
    float playerRot = 0.0f;

    // TODO: Side data struct. Held by sector
    // Holds: sector, texture, etc.

    std::vector<Sector> sectors = {
        {3.0, 1.0, {255, 0, 0, 255}, {255, 0, 0, 255}},
        {1.0, 6.0, {0, 255, 0, 255}, {0, 255, 0, 255}},
        {1.0, 1.0, {0, 0, 255, 255}, {0, 0, 255, 255}}
    };

    std::vector<Line> lines = {
        // Pay attention to point order. Dictates which side is visible side
        {true, {8,0},{0,0}, &sectors[0], &sectors[0]}, // 0
        {true, {0,0},{0,8}, &sectors[0], &sectors[0]}, // 1
        {true, {0,8},{8,8}, &sectors[0], &sectors[0]}, // 2
        {true, {8,8},{8,6}, &sectors[0], &sectors[0]}, // 3
        {false,{8,4},{8,6}, &sectors[0], &sectors[2]}, // 4   DOOR
        {true, {8,4},{8,0}, &sectors[0], &sectors[0]}, // 5 // END SECTOR 1
        {true, {18,0},{10,0}, &sectors[1], &sectors[1]}, // 6
        {true, {18,0},{18,8}, &sectors[1], &sectors[1]}, // 7
        {true, {10,8},{18,8}, &sectors[1], &sectors[1]}, // 8
        {true, {10,8},{10,6}, &sectors[1], &sectors[1]}, // 9
        {false,{10,6},{10,4}, &sectors[1], &sectors[2]}, // 10    DOOR
        {true, {10,4},{10,0}, &sectors[1], &sectors[1]}, // 11 // END SECTOR 2
        {true, {8,4}, {10,4}, &sectors[2], &sectors[2]},  // 12
        {true, {8,6}, {10,6}, &sectors[2], &sectors[2]},  // 13 // END DOOR WALLS
    };

    Uint64 t_0 = SDL_GetPerformanceCounter();
    Uint64 ft_curr = t_0;
    Uint64 ft_prev = 0;
    double dt_s = 0;
    double dt_total = 0;

    bool running = true;
    while(running)
    {
        ft_prev = ft_curr;
        ft_curr = SDL_GetPerformanceCounter();
        dt_s = (double)((ft_curr - ft_prev) / (double) SDL_GetPerformanceFrequency());
        dt_total += dt_s;

        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_KEYDOWN :
                switch(event.key.keysym.sym)
                {
                case SDLK_ESCAPE :
                    running = false;
                    break;

                case SDLK_m :
                    if (SDL_GetRelativeMouseMode() == SDL_TRUE)
                        SDL_SetRelativeMouseMode(SDL_FALSE);
                    else
                        SDL_SetRelativeMouseMode(SDL_TRUE);
                    break;

                case SDLK_PAGEUP :
                    FOV += 10 * 3.1415/180.0f;
                    if (FOV > 360 * 3.1415/180.0f) FOV = 360 * 3.1415/180.0f;
                    break;

                case SDLK_PAGEDOWN :
                    FOV -= 10 * 3.1415/180.0f;
                    if (FOV <= 30 * 3.1415/180.0f) FOV = 30 * 3.1415/180.0f;
                    break;
                }
                break;

            case SDL_MOUSEWHEEL :
                MAPZOOM += 0.1 * event.wheel.y;
                if (MAPZOOM < 0) MAPZOOM = 0;
                break;
            }
        }

        int mouseRelMotionX;
        SDL_GetRelativeMouseState(&mouseRelMotionX, NULL);
        playerRot += MOUSE_SENSITIVITY * mouseRelMotionX;

        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        if(keystate[SDL_SCANCODE_W])
        {
            playerPos = playerPos + vec2{ (float) dt_s * cos(playerRot), (float) dt_s * sin(playerRot)} * PLAYERSPEED;
        }
        if(keystate[SDL_SCANCODE_A])
        {
            playerPos = playerPos + vec2{ (float) dt_s * cos(playerRot-3.1415f/2), (float) dt_s * sin(playerRot-3.1415f/2)} * PLAYERSPEED;
        }
        if(keystate[SDL_SCANCODE_S])
        {
            playerPos = playerPos - vec2{ (float) dt_s * cos(playerRot), (float) dt_s * sin(playerRot)} * PLAYERSPEED;
        }
        if(keystate[SDL_SCANCODE_D])
        {
            playerPos = playerPos - vec2{ (float) dt_s * cos(playerRot-3.1415f/2), (float) dt_s * sin(playerRot-3.1415f/2)} * PLAYERSPEED;
        }
        if(keystate[SDL_SCANCODE_LSHIFT])
        {
            playerZ -= dt_s * PLAYERSPEED;
        }
        if(keystate[SDL_SCANCODE_SPACE])
        {
            playerZ += dt_s * PLAYERSPEED;
        }

        // UPDATE

        sectors[2].floorHeight += dt_s * sin(dt_total);

        Sector* pCurrSec;
        for (Sector sec : sectors) {
            if (sec.isCurrentSector(playerPos, playerRot, lines)) {
                pCurrSec = &sec;
                break;
            }
        }

        // END UPDATE

        mapWindow.clear({0, 0, 0, 255});

        // Render map
        mapWindow.setTitle("Map: Location: " + std::to_string(int(playerPos.x)) + ", " + std::to_string(int(playerPos.y)));
        mapWindow.drawLine(MAP_WIDTH_HEIGHT/2, 0, MAP_WIDTH_HEIGHT/2, MAP_WIDTH_HEIGHT, {255, 255, 0, 255});
        mapWindow.drawLine(0, MAP_WIDTH_HEIGHT/2, MAP_WIDTH_HEIGHT, MAP_WIDTH_HEIGHT/2, {255, 255, 0, 255});
        vec2 arrow{-cos(playerRot)*MAPZOOM + MAP_WIDTH_HEIGHT/2, -sin(playerRot)*MAPZOOM + MAP_WIDTH_HEIGHT/2};
        mapWindow.drawLine(MAP_WIDTH_HEIGHT/2, MAP_WIDTH_HEIGHT/2, arrow.x, arrow.y, {255, 255, 0, 255});

        for (Line line : lines)
        {
            RGBA clr {255,255,255,255};
            if (!line.isFullWall) clr = RGBA{100,100,100,255};
            mapWindow.drawLine(MAPZOOM * (playerPos.x - line.p1.x) + HEIGHT/2, MAPZOOM * (playerPos.y - line.p1.y) + HEIGHT/2,
                               MAPZOOM * (playerPos.x - line.p2.x) + HEIGHT/2, MAPZOOM * (playerPos.y - line.p2.y) + HEIGHT/2,
                               clr);
        }

        for (int x = 0; x < WIDTH; x++)
        {
            float radians = (playerRot) + atan(float(WIDTH/HEIGHT) * (FOV) * (float(x-WIDTH/2)/float(WIDTH/2)));
            Ray ray({playerPos, {cos(radians), sin(radians)} });

            std::vector<HitRecord> intersections;

            for (Line line : lines)
            {
                vec2 point;
                int whichSide;
                if (line.intersect(ray, &point))
                {
                    whichSide = line.getSector(ray);
                    float eucDist = sqrt((playerPos.x-point.x)*(playerPos.x-point.x) + (playerPos.y-point.y)*(playerPos.y-point.y));
                    float dist = eucDist * cos(radians - playerRot);
                    intersections.push_back({dist, line, whichSide});
                }
            }

            std::sort(intersections.begin(), intersections.end(), sortByDist);

            // Draw intersections in order. TODO: EFFICIENTLY TEST STUFF BEFORE JUST STUPIDLY APPLYING PAINTERS ALGORITHM
            for (auto it = intersections.rbegin(); it != intersections.rend(); it++)
            {
                drawWall(mainWindow, x, it->line, it->whichSide, it->dist, playerZ);
            }

        }
        mainWindow.render();
        mapWindow.render();
    }
    SDL_Quit();
    return 0;
}






















