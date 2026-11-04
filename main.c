#include "raylib.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#define screenwidth 640
#define screenheight 480
#define mapwidth 24
#define mapheight 24

// world map
// 0 - free area
// 1 - walls
// 2 - small room
// 3 - pillars
// 4 - corridor with a room

int worldmap[mapwidth][mapheight] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int main() {
    double posX = 22, posY = 12;
    double dirX = -1, dirY = 0;
    double planeX = 0, planeY = 0.66;   //camera plane

    // double time = 0;    //curr frame time            // not needed, ray lib has builting function
    // double prevtime = 0;    //prev frame time
    //


    InitWindow(screenwidth, screenheight, "Retro-Cast");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        double frametime = GetFrameTime();

        for(int x = 0; x <screenwidth; x++) {
            // calculate ray position and direction
            double cameraX =2*x/ (double)screenwidth-1;
            double rayDirX = dirX + planeX *cameraX;
            double rayDirY = dirY + planeY *cameraX;

            double rayDir = sqrt(rayDirX*rayDirX + rayDirY*rayDirY);    // we can ignore this and use 1 inplace of this cause only ratio matters.
                                                                        // But ill write it anyways for learning purpose.(Remove it later if you wanna optimise it)
            // double deltaDistX = fabs(rayDir/rayDirX);
            // double deltaDistY = fabs(rayDir/rayDirY);        <----- cant do this because it will lead to division by zeros
            double deltaDistX;
            if(rayDirX == 0)    deltaDistX = 1e30; // setting it to some high number
            else deltaDistX = fabs(rayDir/rayDirX);
            double deltaDistY;
            if(rayDirY == 0)    deltaDistY = 1e30; // setting it to some high number
            else deltaDistY = fabs(rayDir/rayDirY);

            // initiating the ray
            int mapX = (int)posX;
            int mapY = (int)posY;

            double sideDistX;
            double sideDistY;

            double perpWallDist;    // will be used later to calculate length of ray

            //telling which direction the ray should go( basically velocity). +1 or -1
            int stepX;
            int stepY;

            bool hit = 0; // checks if theres a wall hit
            int side;   // which side of wall are we on, NS(1) or EW(0)

            // calculating step and initial side distances
            if(rayDirX < 0) {
                stepX = -1;
                sideDistX = (posX - mapX)*deltaDistX;
            }
            else {
                stepX = 1;
                sideDistX = (1- posX + mapX)*deltaDistX;
            }
            if(rayDirY < 0) {
                stepY = -1;
                sideDistY = (posY - mapY)*deltaDistY;
            }
            else {
                stepY = 1;
                sideDistY = (1-posY + mapY)*deltaDistY;
            }

            // actual DDA stuff
            while(hit == 0) {
                // jump to next square x or y, which ever closest
                if(sideDistX < sideDistY) {
                    sideDistX += deltaDistX;
                    mapX +=stepX;
                    side = 0;
                }
                else {
                    sideDistY += deltaDistY;
                    mapY +=stepY;
                    side = 1;
                }

                //check if ray is hitting the wall
                if(worldmap[mapX][mapY] > 0) hit = 1;
            }
            if(side == 0) perpWallDist = (sideDistX - (deltaDistX/rayDir));
            else if(side == 1) perpWallDist = (sideDistY - (deltaDistY/rayDir));

            //calculate height of wall
            int lineHeight = (int)(screenheight/perpWallDist);  // multiply this by some number if u see too many boxes or too less boxes
            //Calculate lowest and highest pixel to fill in current stripe
            int drawStart = -lineHeight/2 + screenheight/2;
            if(drawStart < 0) drawStart = 0;
            int drawEnd = lineHeight/2 + screenheight/2;
            if(drawEnd > screenheight) drawEnd = screenheight;

            //wall colors
            Color color;
            switch (worldmap[mapX][mapY]) {
                case 1:color = RED; break;
                case 2:color = GREEN; break;
                case 3:color = BLUE; break;
                case 4:color = WHITE; break;
                default:color = YELLOW; break;
            }

            // seperating x and y walls by giving different colors(gives a show like effect)
            if(side == 1) {
                color = (Color) {color.r/2, color.g/2, color.b/2, color.a/2};
            }
            DrawLine(x, drawStart, x, drawEnd, color);
        }

        //FPS counter, Not need fps will be 60 only anyways(but looks good to have fps displayed on screen)
        DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 20, GREEN);   //not sure if this will work(copied this line from AI)
        EndDrawing();

        //Input stuff
        double movespeed = frametime * 5.0;
        double rotspeed = frametime * 3.0;

        if(IsKeyDown(KEY_UP)) {
            if(worldmap[(int)(posX + dirX * movespeed)][(int)(posY)] == 0) posX += dirX * movespeed;
            if(worldmap[(int)(posX)][(int)(posY + dirY * movespeed)] == 0) posY += dirY * movespeed;
        }
        if(IsKeyDown(KEY_DOWN))
        {
            if(worldmap[(int)(posX - dirX * movespeed)][(int)(posY)] == 0) posX -= dirX * movespeed;
            if(worldmap[(int)(posX)][(int)(posY - dirY * movespeed)] == 0) posY -= dirY * movespeed;

        }

        //implement Rotation;
    }

    CloseWindow();
    return 0;
}
