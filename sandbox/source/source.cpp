#define STB_IMAGE_IMPLEMENTATION

#include <sdl3/sdl.h>
#include <stb_image.h>

import std;
import rigid;

class Benchmark {
public:
    // Simple single-run benchmark
    template<typename Func>
    static double time(Func&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start).count();
    }

    // Multi-run benchmark with statistics
    template<typename Func>
    static void run(const std::string& name, Func&& func, int iterations = 100) {
        std::vector<double> times;
        times.reserve(iterations);

        // Warmup run
        func();

        // Benchmark runs
        for (int i = 0; i < iterations; ++i) {
            times.push_back(time(func));
        }

        printStats(name, times);
    }

private:
    static void printStats(const std::string& name, std::vector<double>& times) {
        std::sort(times.begin(), times.end());

        double sum = std::accumulate(times.begin(), times.end(), 0.0);
        double mean = sum / times.size();

        double sq_sum = 0.0;
        for (auto t : times) {
            sq_sum += (t - mean) * (t - mean);
        }
        double stddev = std::sqrt(sq_sum / times.size());

        double median = times[times.size() / 2];
        double min = times.front();
        double max = times.back();

        std::cout << "\n=== " << name << " ===\n";
        std::cout << "Iterations: " << times.size() << "\n";
        std::cout << "Mean:       " << mean << " ms\n";
        std::cout << "Median:     " << median << " ms\n";
        std::cout << "Std Dev:    " << stddev << " ms\n";
        std::cout << "Min:        " << min << " ms\n";
        std::cout << "Max:        " << max << " ms\n";
    }
};

void load_stb(char const* const file)
{
    int width, height, channels;
    unsigned char* img = stbi_load(file, &width, &height, &channels, 0);
    stbi_image_free(img);
}
//void test_images()
//{
//    auto idk = [](std::filesystem::path path) -> rgd::image
//        {
//            auto       file       = std::ifstream{ path, std::ios::binary };
//            if (!file) throw;
//
//            auto const string     = std::string{ std::istreambuf_iterator<rgd::char_t>(file), std::istreambuf_iterator<rgd::char_t>() };
//            auto const image_data = std::span<const std::uint8_t>{ std::bit_cast<const std::uint8_t*>(string.data()), string.size() };
//
//            return rgd::decode(std::nullopt, image_data);
//        };
//
//    auto const test_directory  = std::filesystem::path{ "test\\primary"       };
//    auto const check_directory = std::filesystem::path{ "test\\primary_check" };
//
//    for (auto const& entry : std::filesystem::directory_iterator{ test_directory })
//    {
//        auto first  = idk(entry.path());
//        auto second = idk(check_directory / entry.path().filename());
//
//        if (!std::memcmp(first.data.data(), second.data.data(), std::min(first.data.size(), second.data.size())))
//        {
//            std::println("image data does not match");
//            __debugbreak();
//        }
//    }
//}
int main()
{
    auto const file_name  = "images/mable_rgb.png";
    auto       file       = std::ifstream{ file_name, std::ios::binary };
    if (!file) throw;

    auto const string     = std::string{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
    auto const image_data = std::span<const std::uint8_t>{ std::bit_cast<const std::uint8_t*>(string.data()), string.size() };
    auto const image      = rgd::decode(std::nullopt, image_data);



    Benchmark::run("STB", [&]() { load_stb(file_name); }, 100);
    Benchmark::run("RGD", [&]() { rgd::decode(rgd::image_layout_e::rgb, image_data); }, 100);



    auto const pixel_format = image.layout == rgd::image_layout_e::rgb ? SDL_PIXELFORMAT_RGB24 : SDL_PIXELFORMAT_RGBA32;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window*   window   = SDL_CreateWindow("SDL3 Texture Example", image.dimensions.x, image.dimensions.y, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    SDL_Texture*  texture  = SDL_CreateTexture(renderer, pixel_format, SDL_TEXTUREACCESS_STATIC, image.dimensions.x, image.dimensions.y);
    SDL_UpdateTexture(texture, NULL, image.data.data(), image.dimensions.x * std::to_underlying(image.layout));

    auto running = true;
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
