
#include <SDL.h>
#undef main
#include <iostream>
#include <numeric>
#include <complex>

int WIDTH = 800;
int HEIGHT = 800;

long double min = -2.0;
long double max = 2.0;
int MAX_ITERATIONS = 200;

long double map(long double value, long double in_min, long double in_max, long double out_min, long double out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window;
    SDL_Renderer* renderer;

    SDL_CreateWindowAndRenderer(1920, 1080, 0, &window, &renderer); 
    SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);
    SDL_Event event;
    while (true)
    {
        SDL_RenderPresent(renderer);

        for (int x = 0; x < WIDTH; x++)
        {
            for (int y = 0; y < HEIGHT; y++)
            {
                long double a = map(x, 0, WIDTH, min, max);
                long double b = map(y, 0, HEIGHT, min, max);

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
                if (n == MAX_ITERATIONS)
                {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderDrawPoint(renderer, x, y);



                }

            }
        }

    }
}
