#include <stdlib.h>
#include <stdio.h>

#include <cmath>
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <chrono>
#include <random>

#include "Screen.h"

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000

const int sectorSize = 50;

int halfWidth = WINDOW_WIDTH/2;
int halfHeight = WINDOW_HEIGHT/2;

constexpr int amountOfPixels = WINDOW_WIDTH*WINDOW_HEIGHT;

uint32_t pixelBuffer[amountOfPixels];

float randomVectors[WINDOW_HEIGHT/sectorSize + 1][WINDOW_WIDTH/sectorSize + 1];

float a = 0;

static void placePoint(int x,int y,uint32_t colour) {
    pixelBuffer[y * WINDOW_WIDTH + x] = colour;
}

float dot(float dx, float dy, float angle) {
    return (dx * std::cos(angle) + dy * std::sin(angle));
}

static float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

static float fade(float x) {
    return x* x * x * (x * (x * 6 - 15) + 10);
}

static uint32_t floatToRGBA32(float value) {
    value +=1;
    value /= 2;

    uint8_t gray = static_cast<uint8_t>(value * 255.0f);

    uint32_t rgba = (gray << 24) | (gray << 16) | (gray << 8) | 255;

    return rgba;
}

static void perlin(){ 
    for (int sectorY = 0; sectorY < WINDOW_HEIGHT/sectorSize; sectorY++)
    {
        for (int sectorX = 0; sectorX < WINDOW_WIDTH/sectorSize; sectorX++)
        {
            for (int y = 0; y < sectorSize; y++)
            {
                for (int x = 0; x < sectorSize; x++)
                {
                    float dx = x / float(sectorSize);
                    float dy = y / float(sectorSize);
                    
                    float bottomLeft = dot(dx,dy,randomVectors[sectorX][sectorY]+a);

                    float topLeft = dot(dx,dy-1.0f,randomVectors[sectorX][sectorY+1]+a);

                    float bottomRight = dot(dx-1.0f,dy,randomVectors[sectorX+1][sectorY]+a);

                    float topRight = dot(dx-1.0f,dy-1.0f,randomVectors[sectorX+1][sectorY+1]+a);

                    float sx = fade(dx);
                    float sy = fade(dy);

                    float interpolatedBottom = lerp(bottomLeft,bottomRight,sx);
                    float interpolatedTop = lerp(topLeft,topRight,sx);
                                        
                    float interpolated = lerp(interpolatedBottom,interpolatedTop,sy);

                    placePoint(sectorX*sectorSize+x,sectorY*sectorSize+y,floatToRGBA32(interpolated));
                }
            }
        }
    }
}

void resetBuffer(){
    for(int i = 0; i < amountOfPixels; i++){
        pixelBuffer[i] = 0x000000FF;
    }
}

int main(){
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<float> dist(0.0, 6.28318531);
    
    for (int y = 0; y < (WINDOW_HEIGHT/sectorSize + 1); y++)
    {
        for (int x = 0; x < (WINDOW_WIDTH/sectorSize + 1); x++)
        {
            randomVectors[x][y] = dist(gen);
        }
    }

    Screen mainScreen(WINDOW_WIDTH, WINDOW_HEIGHT);

    while(!glfwWindowShouldClose(mainScreen.window)){
        
        resetBuffer();
        
        auto start = std::chrono::high_resolution_clock::now();
        
        a += 0.1;
        perlin();

        mainScreen.displayBuffer(pixelBuffer);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << duration.count() << ":microseconds "  <<  1000000.0f/duration.count()  << ":FPS" << std::endl;
    }
     
}