export module rigid.decode.png;

import std;
import <zlib-ng/zlib-ng.h>;
import "sdl3/sdl.h";

export namespace std
{
    using bool_t = bool;
    using byte_t = std::uint8_t;
}
export namespace rgd
{
    auto inflate(const std::vector<std::byte_t>& compressedData) -> std::vector<std::byte_t>
    {
        auto stream = zng_stream{};
        auto result = std::int32_t{};
        
        result = zng_inflateInit(&stream);
        if (result != Z_OK) throw std::runtime_error("Failed to initialize zlib-ng inflation: " + std::to_string(result));

        stream.next_in  = compressedData.data();
        stream.avail_in = static_cast<std::uint32_t>(compressedData.size());

              auto decompressed_data = std::vector<std::byte_t>{};
        const auto chunk_size        = size_t{ 16384u };

        do 
        {
            const auto old_size = std::size_t{ decompressed_data.size() };
            decompressed_data.resize(old_size + chunk_size);

            stream.next_out  = decompressed_data.data() + old_size;
            stream.avail_out = chunk_size;

            result = zng_inflate(&stream, Z_NO_FLUSH);

            if (result != Z_OK && result != Z_STREAM_END) 
            {
                zng_inflateEnd(&stream);
                throw std::runtime_error("Failed to inflate data: " + std::to_string(result));
            }

            decompressed_data.resize(old_size + chunk_size - stream.avail_out);

        } while (result != Z_STREAM_END);

        zng_inflateEnd(&stream);
        
        return decompressed_data;
    }



    template<typename T>
    auto interpret_as(      std::byte_t* pointer, const std::size_t offset = 0u) -> T&
    {
        return *std::bit_cast<T*>(pointer + offset);
    }
    template<typename T>
    auto view_as     (const std::byte_t* pointer, const std::size_t offset = 0u) -> const T&
    {
        return *std::bit_cast<const T*>(pointer + offset);
    }
    template<typename T>
    auto byte_swap   (const T& value) -> T
    {
        if   constexpr (std::is_enum_v<T>) return static_cast<T>(std::byteswap(std::to_underlying(value)));
        else                               return                std::byteswap(                   value  );
    }



    template<typename... Ts>
    struct overload : Ts...
    {
        using Ts::operator()...;
    };



    template<typename T, std::endian E>
    class endian_view
    {
    public:
        auto to_native() const -> T
        {
            if   constexpr (E == std::endian::native) return                value_;
            else                                      return rgd::byte_swap(value_);
        }
        operator T() const
        {
            return to_native();
        }

    private:
        T value_;
    };
    template<typename T>
    using little_endian_view = endian_view<T, std::endian::little>;
    template<typename T>
    using big_endian_view    = endian_view<T, std::endian::big>;

    

    namespace png
    {
        enum class chunk_e : std::uint32_t
        {
            ihdr = 0x49'48'44'52, 
            plte = 0x50'4C'54'45, 
            idat = 0x49'44'41'54, 
            iend = 0x49'45'4E'44, 

            chrm = 0x63'48'52'4D, 
            gama = 0x67'41'4D'41, 
            sbit = 0x73'42'49'54, 
            bkgd = 0x62'4B'47'44, 
            hist = 0x68'49'53'54, 
            trns = 0x74'52'4E'53, 
            phys = 0x70'48'59'73, 
            time = 0x74'49'4D'45, 
            text = 0x74'45'58'74, 
            ztxt = 0x7A'54'58'74, 
        };
        enum class bit_depth_e : std::uint8_t
        {
            _1  =  1u, 
            _2  =  2u, 
            _4  =  4u, 
            _8  =  8u, 
            _16 = 16u, 
        };
        enum class color_type_e : std::uint8_t
        {
            none          = 0u, 
            palette       = 1u, 
            color         = 2u, 
            color_palette = color | palette, 
            alpha         = 4u, 
            color_alpha   = color | alpha, 
        };
        enum class compression_method_e : std::uint8_t
        {
            _0, 
        };
        enum class filter_method_e : std::uint8_t
        {
            _0, 
        };
        enum class filter_e : std::uint8_t
        {
            none     = 0u, 
            subtract = 1u, 
            up       = 2u, 
            average  = 3u, 
            paeth    = 4u, 
        };
        enum class interlace_method_e : std::uint8_t
        {
            _0, 
            _1, 
        };

        struct header
        {
            header(const std::byte_t* data)
                : length{ rgd::byte_swap(rgd::view_as<std::uint32_t>(data,          0u)) }
                , type  { rgd::byte_swap(rgd::view_as<png::chunk_e >(data,          4u)) }
                , crc   { rgd::byte_swap(rgd::view_as<std::uint32_t>(data, length + 8u)) }
            {}

            std::uint32_t length;
            png::chunk_e  type;
            std::uint32_t crc;
        };

        #pragma pack(push, 1u)
        struct ihdr_data
        {
            rgd::big_endian_view<std::uint32_t            > width;
            rgd::big_endian_view<std::uint32_t            > height;
            rgd::big_endian_view<png::bit_depth_e         > bit_depth;
            rgd::big_endian_view<png::color_type_e        > color_type;
            rgd::big_endian_view<png::compression_method_e> compression_method;
            rgd::big_endian_view<png::filter_method_e     > filter_method;
            rgd::big_endian_view<png::interlace_method_e  > interlace_method;
        };
        struct plte_data
        {
            struct entry
            {
                rgd::big_endian_view<std::uint8_t> red;
                rgd::big_endian_view<std::uint8_t> green;
                rgd::big_endian_view<std::uint8_t> blue;
            };

            std::span<const entry> entries;
        };
        struct idat_data
        {
            const std::byte_t* data;
        };
        struct iend_data
        {

        };
        struct chrm_data
        {
            rgd::big_endian_view<std::uint32_t> white_point_x;
            rgd::big_endian_view<std::uint32_t> white_point_y;
            rgd::big_endian_view<std::uint32_t> red_x;
            rgd::big_endian_view<std::uint32_t> red_y;
            rgd::big_endian_view<std::uint32_t> green_x;
            rgd::big_endian_view<std::uint32_t> green_y;
            rgd::big_endian_view<std::uint32_t> blue_x;
            rgd::big_endian_view<std::uint32_t> blue_y;
        };
        struct gama_data
        {
            rgd::big_endian_view<std::uint32_t> gamma;
        };
        struct sbit_data
        {

        };
        struct bkgd_data
        {
            struct indexed_color
            {
                rgd::big_endian_view<std::uint8_t> value;
            };
            struct gray_scale
            {
                rgd::big_endian_view<std::uint16_t> value;
            };
            struct true_color
            {
                rgd::big_endian_view<std::uint16_t> red;
                rgd::big_endian_view<std::uint16_t> green;
                rgd::big_endian_view<std::uint16_t> blue;
            };

            std::variant<indexed_color, gray_scale, true_color> color;
        };
        struct hist_data
        {

        };
        struct trns_data
        {

        };
        struct phys_data
        {

        };
        struct time_data
        {

        };
        struct text_data
        {

        };
        struct ztxt_data
        {

        };
        #pragma pack(pop)

        auto paeth_predictor(std::byte_t a, std::byte_t b, std::byte_t c) -> std::byte_t
            {
                const auto p  = a + b - c;
                const auto pa = std::abs(p - a);
                const auto pb = std::abs(p - b);
                const auto pc = std::abs(p - c);

                     if (pa <= pb && pa <= pc) return a;
                else if (pb <= pc            ) return b;
                else                           return c;
            };

        constexpr auto signature = std::uint64_t{ 0x89'50'4E'47'0D'0A'1A'0A };
    };



    class image
    {
    public:
        enum class format_e
        {
            auto_, png, jpeg, bmp, webp, gif, tiff, 
        };
    };



    void assert(const std::bool_t condition, const std::string_view message = "")
    {
        if (!condition) throw std::invalid_argument{ message.data() };
    }
    auto decode_png(std::span<const std::byte_t> image)
    {
        auto cursor  = image.data();



        rgd::assert(image.size() >= sizeof(png::signature));
        if (!std::memcmp(cursor, &png::signature, sizeof(png::signature))) throw std::invalid_argument{ "Invalid png signature!" };
        cursor += sizeof(png::signature);



        auto ihdr_data = png::ihdr_data{};
        auto plte_data = png::plte_data{};
        auto idat_data = png::idat_data{};
        auto iend_data = png::iend_data{};
        auto chrm_data = png::chrm_data{};
        auto gama_data = png::gama_data{};
        auto sbit_data = png::sbit_data{};
        auto bkgd_data = png::bkgd_data{};
        auto hist_data = png::hist_data{};
        auto trns_data = png::trns_data{};
        auto phys_data = png::phys_data{};
        auto time_data = png::time_data{};
        auto text_data = png::text_data{};
        auto ztxt_data = png::ztxt_data{};

        auto deflated_data = std::vector<std::byte_t>{};
        auto text_strings  = std::vector<std::tuple<std::string, std::string>>{};
        auto iend_reached  = std::bool_t{};

        do
        {
            auto header       = png::header{ cursor };
            auto header_debug = rgd::view_as<std::array<char, 4u>>(std::bit_cast<std::byte_t*>(&header.type));

            switch (header.type)
            {
                using enum png::chunk_e;

                case ihdr:
                {
                    ihdr_data = rgd::view_as<png::ihdr_data>(cursor + 8u);
                    break;
                }
                case plte: 
                {
                    if (header.length % sizeof(png::plte_data::entry) != std::uint32_t{ 0u }) throw std::invalid_argument{ "Palette length must be divisible by size of entry!" };
                    
                    const auto entry_count = header.length / sizeof(png::plte_data::entry);
                    plte_data.entries = std::span{ reinterpret_cast<const png::plte_data::entry*>(cursor + 8u), entry_count };

                    break;
                }
                case idat: 
                {
                    idat_data.data = cursor + 8u;
                    deflated_data.resize(deflated_data.size() + header.length);

                    std::memcpy(deflated_data.data(), idat_data.data, header.length);

                    break;
                }
                case iend:
                {
                    iend_reached = true;
                    break;
                }
                
                case chrm:
                {
                    chrm_data = rgd::view_as<png::chrm_data>(cursor + 8u);
                    break;
                }
                case gama: 
                {
                    gama_data = rgd::view_as<png::gama_data>(cursor + 8u);
                    break;
                }
                case sbit: break;
                case bkgd: 
                {
                    switch (ihdr_data.color_type)
                    {
                        case png::color_type_e::color_palette:
                        {
                            bkgd_data.color = rgd::view_as<png::bkgd_data::indexed_color>(cursor + 8u);
                            break;
                        }
                        case png::color_type_e::none         :
                        case png::color_type_e::alpha        :
                        {
                            bkgd_data.color = rgd::view_as<png::bkgd_data::gray_scale   >(cursor + 8u);
                            break;
                        }
                        case png::color_type_e::color        :
                        case png::color_type_e::color_alpha  :
                        {
                            bkgd_data.color = rgd::view_as<png::bkgd_data::true_color   >(cursor + 8u);
                            break;
                        }
                    }

                    break;
                }
                case hist: break;
                case trns: break;
                case phys: break;
                case time: break;
                case text: 
                {
                    auto key       = std::string{ reinterpret_cast<const char*>(cursor + 8u), header.length };
                    auto separator = key.find('\0') + std::size_t{ 1u };
                    auto value     = key.substr(separator, header.length - separator);

                    text_strings.emplace_back(std::move(key), std::move(value));

                    break;
                }
                case ztxt: break;
            }

            cursor += sizeof(png::header) + header.length;
        } while (!iend_reached);







        const auto bytes_per_pixel    = std::size_t{ 3u };
        const auto scanline_data_size = std::size_t{ ihdr_data.width    * 3u };
        const auto scanline_size      = std::size_t{ scanline_data_size + 1u };
        const auto image_data_size    = scanline_data_size * ihdr_data.height;

              auto inflated_data      = rgd::inflate(deflated_data);
              auto input_image        = std::vector<std::uint8_t >(image_data_size );
              auto output_image       = std::vector<std::uint8_t >(image_data_size );

        std::ranges::for_each(std::views::iota(0u, ihdr_data.height), [&](const auto row_index)
            {
                const auto scanline_start = scanline_size * row_index;
                const auto scanline       = std::span{ inflated_data.data() + scanline_start, scanline_size };
                const auto filter         = static_cast<png::filter_e>(scanline[0u]);

                std::ranges::copy(scanline.subspan(1u), input_image.begin() + (row_index * scanline_data_size));

                      auto input          = std::span<const std::byte_t>{ input_image .data() + ( row_index * scanline_data_size), scanline_data_size };
                      auto output         = std::span<      std::byte_t>{ output_image.data() + ( row_index * scanline_data_size), scanline_data_size };
                      auto prior          = (row_index > 0u) ? std::span<const std::byte_t>{ output_image.data() + ((row_index - 1) * scanline_data_size), scanline_data_size } : input;

                switch (filter)
                {
                    using enum png::filter_e;

                    case none    :
                    {
                        for (auto index = std::size_t{ 0u }; index < input.size(); ++index)
                        {
                            output[index] = input[index];
                        }

                        break;
                    }
                    case subtract:
                    {
                        for (auto index = std::size_t{ 0u }; index < input.size(); ++index)
                        {
                            const auto left = std::byte_t{ (index >= bytes_per_pixel) ? output[index - bytes_per_pixel] : 0u };
                        
                            output[index] = input[index] + left;
                        }

                        break;
                    }
                    case up      :
                    {
                        for (auto index = std::size_t{ 0u }; index < input.size(); ++index)
                        {
                            const auto up = std::byte_t{ (index < prior.size()) ? prior[index] : 0u };
                        
                            output[index] = input[index] + up;
                        }

                        break;
                    }
                    case average :
                    {
                        for (auto index = std::size_t{ 0u }; index < input.size(); ++index)
                        {
                            const auto left    = std::byte_t{ (index >= bytes_per_pixel) ? output[index - bytes_per_pixel] : 0u };
                            const auto up      = std::byte_t{ (index <  prior.size()   ) ? prior [index                  ] : 0u };
                            const auto average = std::byte_t{ (left + up) / 2u };
                        
                            output[index] = input[index] + average;
                        }

                        break;
                    }
                    case paeth   :
                    {
                        for (auto index = std::size_t{ 0u }; index < input.size(); ++index)
                        {
                            const auto left    = std::byte_t{ (index >= bytes_per_pixel                        ) ? output[index - bytes_per_pixel] : 0u };
                            const auto up      = std::byte_t{ (index <  prior.size()                           ) ? prior [index                  ] : 0u };
                            const auto up_left = std::byte_t{ (index >= bytes_per_pixel && index < prior.size()) ? prior [index - bytes_per_pixel] : 0u };
                            const auto paeth   = png::paeth_predictor(left, up, up_left);

                            output[index] = input[index] + paeth;
                        }

                        break;
                    }
                }
            });



        SDL_Init(SDL_INIT_VIDEO);
        int width  = ihdr_data.width;
        int height = ihdr_data.height;
        SDL_Window* window     = SDL_CreateWindow("SDL3 Texture Example", width, height, SDL_WINDOW_RESIZABLE);
        SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
        SDL_Texture* texture   = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STATIC, width, height);
        SDL_UpdateTexture(texture, NULL, output_image.data(), width * 3);

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
        SDL_DestroyTexture (texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow  (window);
        SDL_Quit           ();

        return 0;
    }
    template<image::format_e F = image::format_e::auto_>
    auto decode(std::span<const std::byte_t> image)
    {
        decode_png(image);
        return image;
    }
    auto decode_file()
    {

    }
}
