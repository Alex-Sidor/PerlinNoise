#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <chrono>
#include <random>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000

const int sectorSize = 50;

int halfWidth = WINDOW_WIDTH/2;
int halfHeight = WINDOW_HEIGHT/2;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* texture = NULL;

constexpr int amountOfPixels = WINDOW_WIDTH*WINDOW_HEIGHT;

Uint32 pixelBuffer[amountOfPixels];

float randomVectors[WINDOW_HEIGHT/sectorSize + 1][WINDOW_WIDTH/sectorSize + 1];

float a = 0;

static void placePoint(int x,int y,Uint32 colour) {
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

static Uint32 floatToRGBA32(float value) {
    value +=1;
    value /= 2;

    uint8_t gray = static_cast<uint8_t>(value * 255.0f);

    Uint32 rgba = (gray << 24) | (gray << 16) | (gray << 8) | 255;

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

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    SDL_SetAppMetadata("Example Renderer Streaming Textures", "1.0", "com.example.renderer-streaming-textures");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Perlin Noise", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!texture) {
        SDL_Log("Couldn't create streaming texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

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

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    auto start = std::chrono::high_resolution_clock::now();
    SDL_memset4(pixelBuffer, 0x000000FF, amountOfPixels);
    a += 0.1;
    perlin();

    SDL_UpdateTexture(texture, NULL, pixelBuffer,WINDOW_WIDTH*sizeof(Uint32));
    
    SDL_RenderTexture(renderer, texture, NULL, NULL);

    SDL_RenderPresent(renderer);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Elapsed Time: " << duration.count() << " microseconds" << std::endl;

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}