#include <sdl3/sdl.h>

import std;
import rigid;

int main()
{
          auto file   = std::ifstream{ "s.png", std::ios::binary };
    const auto string = std::string{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };

    const auto image  = std::span<const std::uint8_t>{ std::bit_cast<const std::uint8_t*>(string.data()), string.size() };
    const auto result = rgd::decode<rgd::image_format_e::png>(image);

    

    SDL_Init(SDL_INIT_VIDEO);
    const int width  = 256;
    const int height = 256;
    SDL_Window*   window   = SDL_CreateWindow("SDL3 Texture Example", width, height, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    SDL_Texture*  texture  = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STATIC, width, height);
    SDL_UpdateTexture(texture, NULL, result.data(), width * 3);

    bool running = true;
    SDL_Event event;
    while (running) 
    {
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_EVENT_QUIT) 
            {
                running = false;
            }
        }
        SDL_SetRenderDrawColor(renderer,       0,    0,    0, 255);
        SDL_RenderClear       (renderer                          );
        SDL_RenderTexture     (renderer, texture, NULL, NULL     );
        SDL_RenderPresent     (renderer                          );
    }

    SDL_DestroyTexture (texture );
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow  (window  );
    SDL_Quit           ();



    return 0;
}
