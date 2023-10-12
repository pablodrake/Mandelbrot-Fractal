
#include <SDL.h>
#include <SDL_ttf.h>
#undef main
#include <Windows.h>
#include <iostream>
#include <numeric>
#include <omp.h>
#include <chrono>
#include <complex>

long double in_min = -1.0, in_max = 1;
long double out_min = -2.5, out_max = 1;
int MAX_ITERATIONS = 64;
double zoom = 1.0;
double zoomFactor = 2;


SDL_Color lerp(SDL_Color color1, SDL_Color color2, double t) {
    double b = 1 - t;
    SDL_Color result;
    result.r = static_cast<Uint8>(b * color1.r + t * color2.r);
    result.g = static_cast<Uint8>(b * color1.g + t * color2.g);
    result.b = static_cast<Uint8>(b * color1.b + t * color2.b);
    return result;
}

// Color function with a multi-colored gradient
SDL_Color getColor(int iterations, int maxIterations) {
    if (iterations == maxIterations) {
        return { 0, 0, 0, 255 }; // Color for points inside the set (usually black)
    }
    else {
        // Define an array of colors for the gradient
        SDL_Color colors[] =
        {
            {0,0,0},
            {213,67,31},
            {251,255,121},
            {62,223,89},
            {43,30,218},
            {0,255,247}

         };

        static const auto maxColor = sizeof(colors) - 1;
        double factor = static_cast<double>(iterations) / static_cast<double>(maxIterations);

        factor *= maxColor;

        auto ifactor = static_cast<size_t>(factor);
        if (ifactor >= maxColor) {
            // Handle the case where 'ifactor' is out of bounds.
            // You can return a default color or take appropriate action.
            return { 0, 0, 0, 255 };
        }

        // Interpolate between two neighboring colors
        SDL_Color color1 = colors[ifactor];
        SDL_Color color2 = colors[min(ifactor + 1, maxColor)];

        SDL_Color interpolatedColor = lerp(color1, color2, factor - ifactor);

        int r = static_cast<int>(interpolatedColor.r);
        int g = static_cast<int>(interpolatedColor.g);
        int b = static_cast<int>(interpolatedColor.b);

        return { static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), 255};
    }
}

void renderMandelbrot(SDL_Renderer* renderer, int screenWidth, int screenHeight)
{
    #pragma omp parallel for
    for (int x = 0; x < screenWidth; x++) {
        for (int y = 0; y < screenHeight; y++) {
            // Adjust a, b, ai, bi based on the zoom level and screen coordinates
            long double a = (out_min + (out_max - out_min) * x / screenWidth);
            long double b = in_min + (in_max - in_min) * y / screenHeight;

            long double ai = 0;
            long double bi = 0;

            int n = 0;

            for (int i = 0; i < MAX_ITERATIONS; i++)
            {
                long double a1 = ai * ai - bi * bi + a;
                bi = 2 * ai*bi + b;

                ai = a1;

                if (ai*ai + bi * bi > 4)
                    break;

                n++;
            }

            SDL_Color pixelColor = getColor(n, MAX_ITERATIONS);
            #pragma omp critical
            {
                SDL_SetRenderDrawColor(renderer, pixelColor.r, pixelColor.g, pixelColor.b, pixelColor.a);
                SDL_RenderDrawPoint(renderer, x, y);
            }

        }
    }

}


int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;


    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm); 
    SDL_Window* window = SDL_CreateWindow("FRACTAL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, dm.w, dm.h, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(renderer, dm.w, dm.h);
    SDL_Event event;
    int xMouse, yMouse;
    while (true) 
    {
        std::string text = "Iterations: " + std::to_string(MAX_ITERATIONS) + "     Zoom: " + std::to_string(zoom);
        TTF_Font* font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 24);
        if (!font)
        {
            std::cout << "xd";
        }
        SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), {255, 255, 255});
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        auto t1 = high_resolution_clock::now();
        renderMandelbrot(renderer, dm.w, dm.h);
        auto t2 = high_resolution_clock::now();
        SDL_Rect destRect;
        destRect.x = 10; // X position
        destRect.y = 10; // Y position
        destRect.w = surface->w; // Width of the text
        destRect.h = surface->h; // Height of the text

        SDL_RenderPresent(renderer);

        // Capture user input
        while (SDL_PollEvent(&event)) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                return 0;
            }
            if (event.type == SDL_MOUSEWHEEL)
            {
                if (event.wheel.y > 0)
                {
                    MAX_ITERATIONS *= 2;
                }
                if (event.wheel.y < 0)
                {
                    MAX_ITERATIONS /= 2;
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                auto zoom_x = [&](double z)
                {
                    SDL_GetGlobalMouseState(&xMouse, &yMouse);
                    // Calculate the new center point
                    double centerX = out_min + (out_max - out_min) * xMouse / dm.w;
                    double centerY = in_min + (in_max - in_min) * yMouse / dm.h;

                    // Calculate new width and height based on the zoom factor
                    double newWidth = (out_max - out_min) / z;
                    double newHeight = (in_max - in_min) / z;

                    // Update the min and max values based on the new center, width, and height
                    out_min = centerX - newWidth / 2;
                    out_max = centerX + newWidth / 2;
                    in_min = centerY - newHeight / 2;
                    in_max = centerY + newHeight / 2;
                };
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    zoom_x(zoomFactor);
                    zoom *= zoomFactor;
                }
                //Right Click to ZoomOut
                if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    zoom_x(1.0 / zoomFactor);
                    zoom /= zoomFactor;
                }
            }
        }
        SDL_RenderCopy(renderer, texture, nullptr, &destRect);
        SDL_RenderPresent(renderer);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        /* Getting number of milliseconds as a double. */
        duration<double, std::milli> ms_double = t2 - t1;

        std::cout << ms_double.count() << "ms\n";

        
    }
}

