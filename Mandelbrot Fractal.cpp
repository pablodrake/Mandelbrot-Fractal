
#include <SDL.h>
#undef main
#include <Windows.h>
#include <iostream>
#include <numeric>
#include <complex>

long double min = -2.0;
long double max = 2.0;
int MAX_ITERATIONS = 200;
double zoom = 1.0; // Initial zoom level
double xOffset = 0.0;

long double map(long double value, long double in_min, long double in_max, long double out_min, long double out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Color function
SDL_Color getColor(int iterations, int maxIterations) {
    if ((iterations == maxIterations) || iterations < 20) {
        return { 0, 0, 0, 255 }; // Color for points inside the set (usually black)
    }
    else {
        double factor = sqrt(static_cast<double>(iterations) / static_cast<double>(maxIterations));
        int intensity = static_cast<int>(255.0 * factor);
        return { static_cast<Uint8>(intensity), static_cast<Uint8>(intensity), static_cast<Uint8>(intensity), 255 };
    }
}

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);



    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm); 
    SDL_Window* window = SDL_CreateWindow("FRACTAL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, dm.w, dm.h, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(renderer, dm.w, dm.h);
    SDL_Event event;
    while (true) 
    {
        SDL_RenderPresent(renderer);

        // Capture user input
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return 0; // Quit when the window is closed
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    return 0; // Quit when the Escape key is pressed
                }
                if (event.key.keysym.sym == SDLK_DOWN) {
                    // Zoom in
                    zoom *= 0.9;
                }
                if (event.key.keysym.sym == SDLK_UP) {
                    // Zoom out
                    zoom /= 0.9;
                }
                if (event.key.keysym.sym == SDLK_LEFT) {
                    // Move to the right
                    xOffset -= 0.1;
                }
                if (event.key.keysym.sym == SDLK_RIGHT) {
                    // Move to the left
                    xOffset += 0.1;
                }
            }
        }

        for (int x = 0; x < dm.w; x++) {
            for (int y = 0; y < dm.h; y++) {
                // Adjust a, b, ai, bi based on the zoom level and screen coordinates
                long double a = map(x, 0, dm.w, min, max) / zoom + xOffset;
                long double b = map(y, 0, dm.h, min, max) / zoom;

                long double ai = a;
                long double bi = b;

                int n = 0;

                for (int i = 0; i < MAX_ITERATIONS; i++)
                {

                    long double a1 = a * a - b * b;
                    long double b1 = 2 * a * b;

                    a = a1 + ai;
                    b = b1 + bi;


                    if ((a + b) > max)
                        break;

                    n++;
                }

                SDL_Color pixelColor = getColor(n, MAX_ITERATIONS);
                SDL_SetRenderDrawColor(renderer, pixelColor.r, pixelColor.g, pixelColor.b, pixelColor.a);
                SDL_RenderDrawPoint(renderer, x, y);


            }
        }

    }
}

