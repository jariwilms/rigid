export module rigid.decode.png;

import std;
import rigid.core;

export namespace rgd::png
{
    constexpr auto signature = 0x89'50'4E'47'0D'0A'1A'0A;

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
        grayscale       = 0u                         , 
        truecolor       = 2u                         , 
        indexed_color   = truecolor | 1u             , 
        grayscale_alpha = 4u                         , 
        truecolor_alpha = truecolor | grayscale_alpha, 
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
        header(const rgd::byte_t* data)
            : length{ rgd::byte_swap(rgd::view_as<std::uint32_t>(data,          0u)) }
            , type  { rgd::byte_swap(rgd::view_as<png::chunk_e >(data,          4u)) }
            , crc   { rgd::byte_swap(rgd::view_as<std::uint32_t>(data, length + 8u)) }
        {}

        std::uint32_t length;
        png::chunk_e  type;
        std::uint32_t crc;
    };
    #pragma pack(push, 1u)
    struct ihdr
    {
        rgd::big_endian_view<std::uint32_t            > width;
        rgd::big_endian_view<std::uint32_t            > height;
        rgd::big_endian_view<png::bit_depth_e         > bit_depth;
        rgd::big_endian_view<png::color_type_e        > color_type;
        rgd::big_endian_view<png::compression_method_e> compression_method;
        rgd::big_endian_view<png::filter_method_e     > filter_method;
        rgd::big_endian_view<png::interlace_method_e  > interlace_method;
    };
    struct plte
    {
        struct entry
        {
            rgd::big_endian_view<std::uint8_t> red;
            rgd::big_endian_view<std::uint8_t> green;
            rgd::big_endian_view<std::uint8_t> blue;
        };

        std::span<const entry> entries;
    };
    struct idat
    {
        const rgd::byte_t* data;
    };
    struct iend
    {

    };
    struct chrm
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
    struct gama
    {
        rgd::big_endian_view<std::uint32_t> gamma;
    };
    struct sbit
    {
        struct grayscale
        {
            rgd::big_endian_view<rgd::byte_t> grayscale_significant_bits;
        };
        struct truecolor
        {
            rgd::big_endian_view<rgd::byte_t> red_significant_bits;
            rgd::big_endian_view<rgd::byte_t> green_significant_bits;
            rgd::big_endian_view<rgd::byte_t> blue_significant_bits;
        };
        struct indexed_color
        {
            rgd::big_endian_view<rgd::byte_t> red_significant_bits;
            rgd::big_endian_view<rgd::byte_t> green_significant_bits;
            rgd::big_endian_view<rgd::byte_t> blue_significant_bits;
        };
        struct grayscale_alpha
        {
            rgd::big_endian_view<rgd::byte_t> grayscale_significant_bits;
            rgd::big_endian_view<rgd::byte_t> alpha_significant_bits;
        };
        struct truecolor_alpha
        {
            rgd::big_endian_view<rgd::byte_t> red_significant_bits;
            rgd::big_endian_view<rgd::byte_t> green_significant_bits;
            rgd::big_endian_view<rgd::byte_t> blue_significant_bits;
            rgd::big_endian_view<rgd::byte_t> alpha_significant_bits;
        };

        std::variant<grayscale, truecolor, indexed_color, grayscale_alpha, truecolor_alpha> color;
    };
    struct bkgd
    {
        struct grayscale
        {
            rgd::big_endian_view<std::uint16_t> value;
        };
        struct truecolor
        {
            rgd::big_endian_view<std::uint16_t> red;
            rgd::big_endian_view<std::uint16_t> green;
            rgd::big_endian_view<std::uint16_t> blue;
        };
        struct indexed_color
        {
            rgd::big_endian_view<std::uint8_t> value;
        };

        std::variant<grayscale, truecolor, indexed_color> color;
    };
    struct hist
    {
        std::span<const std::uint16_t> data;
    };
    struct trns
    {
        struct grayscale
        {
            rgd::big_endian_view<std::uint16_t> value;
        };
        struct truecolor
        {
            rgd::big_endian_view<std::uint16_t> red;
            rgd::big_endian_view<std::uint16_t> green;
            rgd::big_endian_view<std::uint16_t> blue;
        };
        struct indexed_color
        {
            std::span<const rgd::byte_t> values;
        };

        std::variant<grayscale, truecolor, indexed_color> value;
    };
    struct phys
    {

    };
    struct time
    {

    };
    struct text
    {

    };
    struct ztxt
    {

    };
    #pragma pack(pop)

    auto paeth_predictor   (rgd::byte_t a, rgd::byte_t b, rgd::byte_t c) -> rgd::byte_t
        {
            const auto p  = a + b - c;
            const auto pa = std::abs(p - a);
            const auto pb = std::abs(p - b);
            const auto pc = std::abs(p - c);

                 if (pa <= pb && pa <= pc) return a;
            else if (pb <= pc            ) return b;
            else                           return c;
        };
    auto map_color_channels(png::color_type_e color_type) -> std::uint8_t
    {
        switch (color_type)
        {
            case png::color_type_e::grayscale      : return std::uint8_t{ 1u };
            case png::color_type_e::truecolor      : return std::uint8_t{ 3u };
            case png::color_type_e::indexed_color  : return std::uint8_t{ 1u };
            case png::color_type_e::grayscale_alpha: return std::uint8_t{ 2u };
            case png::color_type_e::truecolor_alpha: return std::uint8_t{ 4u };

            default: throw std::invalid_argument{ "invalid color type" };
        }
    }
    
    auto decode            (std::span<const rgd::byte_t> image) -> std::vector<rgd::byte_t>
    {
        auto cursor = image.data();
        if (!std::memcmp(cursor, &png::signature, sizeof(png::signature))) throw std::invalid_argument{ "invalid png signature" };
        cursor += sizeof(png::signature);



        auto ihdr = png::ihdr{};
        auto plte = png::plte{};
        auto idat = png::idat{};
        auto iend = png::iend{};
        auto chrm = png::chrm{};
        auto gama = png::gama{};
        auto sbit = png::sbit{};
        auto bkgd = png::bkgd{};
        auto hist = png::hist{};
        auto trns = png::trns{};
        auto phys = png::phys{};
        auto time = png::time{};
        auto text = png::text{};
        auto ztxt = png::ztxt{};

        auto deflated_data = std::vector<rgd::byte_t>{};
        auto text_strings  = std::vector<std::tuple<std::string, std::string>>{};
        auto iend_reached  = rgd::bool_t{};

        do
        {
            const auto header = png::header{ cursor };
            switch (header.type)
            {
                case png::chunk_e::ihdr:
                {
                    ihdr = rgd::view_as<png::ihdr>(cursor + 8u);
                    break;
                }
                case png::chunk_e::plte: 
                {
                    if (header.length % sizeof(png::plte::entry) != std::uint32_t{ 0u }) throw std::invalid_argument{ "Palette length must be divisible by size of entry!" };
                    
                    const auto entry_count = header.length / sizeof(png::plte::entry);
                    plte.entries           = std::span{ reinterpret_cast<const png::plte::entry*>(cursor + 8u), entry_count };

                    break;
                }
                case png::chunk_e::idat: 
                {
                    idat.data    = cursor + 8u;
                    const auto offset = deflated_data.size();
                    deflated_data.resize(deflated_data.size() + header.length);

                    std::memcpy(deflated_data.data() + offset, idat.data, header.length);

                    break;
                }
                case png::chunk_e::iend:
                {
                    iend_reached = true;
                    break;
                }
                
                case png::chunk_e::chrm:
                {
                    chrm = rgd::view_as<png::chrm>(cursor + 8u);
                    break;
                }
                case png::chunk_e::gama: 
                {
                    gama = rgd::view_as<png::gama>(cursor + 8u);
                    break;
                }
                case png::chunk_e::sbit: 
                {
                    switch (ihdr.color_type)
                    {
                        case png::color_type_e::grayscale      :
                        {
                            sbit.color = rgd::view_as<png::sbit::grayscale>(cursor + 8u);
                            break;
                        }
                        case png::color_type_e::truecolor      :
                        {
                            sbit.color = rgd::view_as<png::sbit::truecolor>(cursor + 8u);
                            break;
                        }
                        case png::color_type_e::indexed_color  :
                        {
                            sbit.color = rgd::view_as<png::sbit::indexed_color>(cursor + 8u);
                            break;
                        }
                        case png::color_type_e::grayscale_alpha:
                        {
                            sbit.color = rgd::view_as<png::sbit::grayscale_alpha>(cursor + 8u);
                            break;
                        }
                        case png::color_type_e::truecolor_alpha:
                        {
                            sbit.color = rgd::view_as<png::sbit::truecolor_alpha>(cursor + 8u);
                            break;
                        }
                    }
                }
                case png::chunk_e::bkgd: 
                {
                    switch (ihdr.color_type)
                    {
                        case png::color_type_e::grayscale      :
                        {
                            bkgd.color = rgd::view_as<png::bkgd::grayscale>(cursor + 8u);
                            break;
                        }
                        case png::color_type_e::truecolor      :
                        {
                            bkgd.color = rgd::view_as<png::bkgd::truecolor   >(cursor + 8u);
                            break;
                        }
                        case png::color_type_e::indexed_color  :
                        {
                            bkgd.color = rgd::view_as<png::bkgd::indexed_color>(cursor + 8u);
                            break;
                        }
                        case png::color_type_e::grayscale_alpha:
                        {
                            bkgd.color = rgd::view_as<png::bkgd::grayscale   >(cursor + 8u);
                            break;
                        }
                        case png::color_type_e::truecolor_alpha:
                        {
                            bkgd.color = rgd::view_as<png::bkgd::truecolor   >(cursor + 8u);
                            break;
                        }
                    }

                    break;
                }
                case png::chunk_e::hist: 
                {
                    hist.data = std::span<const std::uint16_t>{ reinterpret_cast<const std::uint16_t*>(cursor + 8u), plte.entries.size() };
                }
                case png::chunk_e::trns: 
                {
                    switch (ihdr.color_type)
                    {
                        case png::color_type_e::grayscale      :
                        {
                            trns.value = rgd::view_as<png::trns::grayscale>(cursor + 8u);
                            break;
                        }
                        case png::color_type_e::truecolor      :
                        {
                            trns.value = rgd::view_as<png::trns::truecolor>(cursor + 8u);
                            break;
                        }
                        case png::color_type_e::indexed_color  :
                        {
                            trns.value = rgd::view_as<png::trns::indexed_color>(cursor + 8u);
                            break;
                        }
                    }
                }
                case png::chunk_e::phys: break;
                case png::chunk_e::time: break;
                case png::chunk_e::text: 
                {
                    const auto key       = std::string{ reinterpret_cast<const char*>(cursor + 8u), header.length };
                    const auto separator = key.find('\0') + std::size_t{ 1u };
                    const auto value     = key.substr(separator, header.length - separator);

                    text_strings.emplace_back(std::move(key), std::move(value));

                    break;
                }
                case png::chunk_e::ztxt: break;
            }

            cursor += sizeof(png::header) + header.length;
        } while (!iend_reached);

            

        const auto bytes_per_pixel    = png::map_color_channels(ihdr.color_type);
        const auto scanline_data_size = std::size_t{ ihdr.width    * 3u };
        const auto scanline_size      = std::size_t{ scanline_data_size + 1u };
        const auto image_data_size    = scanline_data_size * ihdr.height;

                auto inflated_data      = rgd::inflate(deflated_data);
                auto input_image        = std::vector<std::uint8_t >(image_data_size );
                auto output_image       = std::vector<std::uint8_t >(image_data_size );

        std::ranges::for_each(std::views::iota(0u, ihdr.height), [&](const auto row_index)
            {
                const auto scanline_start = scanline_size * row_index;
                const auto scanline       = std::span{ inflated_data.data() + scanline_start, scanline_size };
                const auto filter         = static_cast<png::filter_e>(scanline[0u]);

                std::ranges::copy(scanline.subspan(1u), input_image.begin() + (row_index * scanline_data_size));

                        auto input          = std::span<const rgd::byte_t>{ input_image .data() + ( row_index * scanline_data_size), scanline_data_size };
                        auto output         = std::span<      rgd::byte_t>{ output_image.data() + ( row_index * scanline_data_size), scanline_data_size };
                        auto prior          = (row_index > 0u) ? std::span<const rgd::byte_t>{ output_image.data() + ((row_index - 1) * scanline_data_size), scanline_data_size } : input;

                switch (filter)
                {
                    case png::filter_e::none    :
                    {
                        for (auto index = std::size_t{ 0u }; index < input.size(); ++index)
                        {
                            output[index] = input[index];
                        }
                        break;
                    }
                    case png::filter_e::subtract:
                    {
                        for (auto index = std::size_t{ 0u }; index < input.size(); ++index)
                        {
                            const auto left = rgd::byte_t{ (index >= bytes_per_pixel) ? output[index - bytes_per_pixel] : 0u };
                        
                            output[index] = input[index] + left;
                        }
                        break;
                    }
                    case png::filter_e::up      :
                    {
                        for (auto index = std::size_t{ 0u }; index < input.size(); ++index)
                        {
                            const auto up = rgd::byte_t{ (index < prior.size()) ? prior[index] : 0u };
                        
                            output[index] = input[index] + up;
                        }
                        break;
                    }
                    case png::filter_e::average :
                    {
                        for (auto index = std::size_t{ 0u }; index < input.size(); ++index)
                        {
                            const auto left    = rgd::byte_t{ (index >= bytes_per_pixel) ? output[index - bytes_per_pixel] : 0u };
                            const auto up      = rgd::byte_t{ (index <  prior.size()   ) ? prior [index                  ] : 0u };
                            const auto average = rgd::byte_t{ (left + up) / 2u };
                        
                            output[index] = input[index] + average;
                        }
                        break;
                    }
                    case png::filter_e::paeth   :
                    {
                        for (auto index = std::size_t{ 0u }; index < input.size(); ++index)
                        {
                            const auto left    = rgd::byte_t{ (index >= bytes_per_pixel                        ) ? output[index - bytes_per_pixel] : 0u };
                            const auto up      = rgd::byte_t{ (index <  prior.size()                           ) ? prior [index                  ] : 0u };
                            const auto up_left = rgd::byte_t{ (index >= bytes_per_pixel && index < prior.size()) ? prior [index - bytes_per_pixel] : 0u };
                            const auto paeth   = png::paeth_predictor(left, up, up_left);

                            output[index] = input[index] + paeth;
                        }
                        break;
                    }
                }
            });

        return output_image;
    }
}
