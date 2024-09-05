#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>

const int screenWidth = 640;
const int screenHeight = 480;
const int mapWidth = 8;
const int mapHeight = 8;

// Simple map: 1 = wall, 0 = empty space
int worldMap[mapWidth][mapHeight] = {
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1}
};

// Player variables
double posX = 1.5, posY = 1.5;  // Player start position
double dirX = 1.0, dirY = 0.0;  // Player direction
double planeX = 0.0, planeY = 0.66;  // 2D camera plane

void render(SDL_Renderer* renderer) {
    for (int x = 0; x < screenWidth; x++) {
        // Calculate ray position and direction
        double cameraX = 2 * x / double(screenWidth) - 1;
        double rayDirX = dirX + planeX * cameraX;
        double rayDirY = dirY + planeY * cameraX;

        // Which box of the map we're in
        int mapX = int(posX);
        int mapY = int(posY);

        // Length of ray from one x or y-side to the next
        double sideDistX;
        double sideDistY;

        // Length of ray from one x or y-side to the next
        double deltaDistX = std::abs(1 / rayDirX);
        double deltaDistY = std::abs(1 / rayDirY);
        double perpWallDist;

        // Step direction
        int stepX;
        int stepY;

        bool hit = false;  // Was there a wall hit?
        int side;  // Was a NS or a EW wall hit?

        // Calculate step and initial sideDist
        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (posX - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - posX) * deltaDistX;
        }
        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (posY - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0 - posY) * deltaDistY;
        }

        // Perform DDA
        while (!hit) {
            // Jump to the next map square in x or y
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            // Check if the ray has hit a wall
            if (worldMap[mapX][mapY] > 0) hit = true;
        }

        // Calculate distance projected on camera direction
        if (side == 0)
            perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
        else
            perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;

        // Calculate height of line to draw on screen
        int lineHeight = (int)(screenHeight / perpWallDist);

        // Calculate lowest and highest pixel to fill in the current stripe
        int drawStart = -lineHeight / 2 + screenHeight / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + screenHeight / 2;
        if (drawEnd >= screenHeight) drawEnd = screenHeight - 1;

        // Choose wall color based on side
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red for walls
        if (side == 1) SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255);  // Darker for y-axis walls

        SDL_RenderDrawLine(renderer, x, drawStart, x, drawEnd);
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Maze Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black for background
        SDL_RenderClear(renderer);

        render(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
