export module rigid.decode.png;

import std;
import zlib_ng;
import rigid.core;

export namespace rgd::png
{
    auto constexpr signature = std::array<rgd::byte_t, 8u>{ 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, };



    template<typename T>
    auto consume(std::span<const rgd::byte_t> memory, rgd::cursor_t& cursor) -> T
    {
        auto const value = rgd::consume<T>(memory, cursor);
        return bit::swap_bytes_native<bit::endian_e::big>(value);
    }
    template<typename T>
    auto consume(std::span<const rgd::byte_t> memory, rgd::cursor_t& cursor, rgd::size_t length) -> T;
    template<>
    auto consume<std::string>(std::span<const rgd::byte_t> memory, rgd::cursor_t& cursor, rgd::size_t length) -> std::string
    {
        auto const string = std::string{ reinterpret_cast<rgd::char_t const*>(memory.data() + cursor), length };
        cursor += length;

        return string;
    }



    enum class ancillary_bit_e : rgd::uint8_t
    {
        critical  = 0u, 
        ancillary = 1u, 
    };
    enum class private_bit_e : rgd::uint8_t
    {
        public_  = 0u, 
        private_ = 1u, 
    };
    enum class reserved_bit_e : rgd::uint8_t
    {
        reserved = 0u, 
    };
    enum class safe_to_copy_bit_e : rgd::uint8_t
    {
        unsafe = 0u, 
        safe   = 1u, 
    };

    enum class chunk_e : std::uint32_t
    {
        bkgd = 0x62'4B'47'44, 
        chrm = 0x63'48'52'4D, 
        gama = 0x67'41'4D'41, 
        hist = 0x68'49'53'54, 
        iccp = 0x69'43'43'50, 
        idat = 0x49'44'41'54, 
        iend = 0x49'45'4E'44, 
        ihdr = 0x49'48'44'52, 
        itxt = 0x69'54'58'74, 
        phys = 0x70'48'59'73, 
        plte = 0x50'4C'54'45, 
        sbit = 0x73'42'49'54, 
        splt = 0x73'50'4C'54, 
        srgb = 0x73'52'47'42, 
        text = 0x74'45'58'74, 
        time = 0x74'49'4D'45, 
        trns = 0x74'52'4E'53, 
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
        grayscale        = 0u                          , 
        true_color       = 2u                          , 
        indexed_color    = true_color | 1u             , 
        grayscale_alpha  = 4u                          , 
        true_color_alpha = true_color | grayscale_alpha, 
    };
    enum class compression_flag_e : rgd::uint8_t
    {
        uncompressed = 0u, 
        compressed   = 1u, 
    };
    enum class compression_method_e : std::uint8_t
    {
        deflate = 0u, 
    };
    enum class filter_method_e : std::uint8_t
    {
        adaptive = 0u, 
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
        none  = 0u, 
        adam7 = 1u, 
    };
    enum class rendering_intent_e : rgd::uint8_t
    {
        perceptual            = 0u, 
        relative_colorimetric = 1u, 
        saturation            = 2u, 
        absolute_colorimetric = 3u, 
    };
    enum class unit_specifier_e : rgd::uint8_t
    {
        unknown = 0u, 
        meter   = 1u, 
    };

    struct chunk_header
    {
        rgd::uint32_t length;
        png::chunk_e  type;
        rgd::uint32_t crc;
    };
    struct chunk
    {
        #pragma pack(push, 1u)
        //Critical Chunks
        struct ihdr
        {
            std::uint32_t             width;
            std::uint32_t             height;
            png::bit_depth_e          bit_depth;
            png::color_type_e         color_type;
            png::compression_method_e compression_method;
            png::filter_method_e      filter_method;
            png::interlace_method_e   interlace_method;
        };
        struct plte
        {
            struct entry
            {
                std::uint8_t red;
                std::uint8_t green;
                std::uint8_t blue;
            };

            std::vector<entry> entries;
        };
        struct idat
        {
            std::vector<rgd::byte_t> data;
        };
        struct iend
        {

        };
        //Ancillary Chunks
        struct trns
        {
            struct grayscale
            {
                rgd::uint16_t level;
            };
            struct true_color
            {
                std::uint16_t red;
                std::uint16_t green;
                std::uint16_t blue;
            };
            struct indexed_color
            {
                std::vector<rgd::uint8_t> entries;
            };

            std::variant<grayscale, true_color, indexed_color> value;
        };
        struct gama
        {
            std::uint32_t gamma;
        };
        struct chrm
        {
            std::uint32_t white_point_x;
            std::uint32_t white_point_y;
            std::uint32_t red_x;
            std::uint32_t red_y;
            std::uint32_t green_x;
            std::uint32_t green_y;
            std::uint32_t blue_x;
            std::uint32_t blue_y;
        };
        struct srgb
        {
            png::rendering_intent_e rendering_intent;
        };
        struct iccp
        {
            std::string               profile_name;
            png::compression_method_e compression_method;
            std::string               compressed_profile;
        };
        struct text
        {
            std::unordered_map<std::string, std::string> map;
        };
        struct ztxt
        {
            std::unordered_map<std::string, std::string> map;
            png::compression_method_e                    compression_method;
        };
        struct itxt
        {
            std::unordered_map<std::string, std::tuple<std::string, std::string>> map;
            png::compression_flag_e                                               compression_flag;
            png::compression_method_e                                             compression_method;
            std::string                                                           language_tag;
        };
        struct bkgd
        {
            struct grayscale
            {
                std::uint16_t level;
            };
            struct true_color
            {
                std::uint16_t red;
                std::uint16_t green;
                std::uint16_t blue;
            };
            struct indexed_color
            {
                std::vector<rgd::uint8_t> entries;
            };

            std::variant<grayscale, true_color, indexed_color> color;
        };
        struct phys
        {
            rgd::uint32_t         pixels_per_unit_x;
            rgd::uint32_t         pixels_per_unit_y;
            png::unit_specifier_e unit_specifier;
        };
        struct sbit
        {
            struct grayscale
            {
                rgd::uint8_t grayscale_significant_bits;
            };
            struct true_color
            {
                rgd::uint8_t red_significant_bits;
                rgd::uint8_t green_significant_bits;
                rgd::uint8_t blue_significant_bits;
            };
            struct indexed_color
            {
                rgd::uint8_t red_significant_bits;
                rgd::uint8_t green_significant_bits;
                rgd::uint8_t blue_significant_bits;
            };
            struct grayscale_alpha
            {
                rgd::uint8_t grayscale_significant_bits;
                rgd::uint8_t alpha_significant_bits;
            };
            struct true_color_alpha
            {
                rgd::uint8_t red_significant_bits;
                rgd::uint8_t green_significant_bits;
                rgd::uint8_t blue_significant_bits;
                rgd::uint8_t alpha_significant_bits;
            };

            std::variant<grayscale, true_color, indexed_color, grayscale_alpha, true_color_alpha> color;
        };
        struct splt
        {
            //std::unordered_map<std::string, std::string> map;
        };
        struct hist
        {
            std::vector<rgd::uint16_t> data;
        };
        struct time
        {
            rgd::uint16_t year;
            rgd::uint8_t  month;
            rgd::uint8_t  day;
            rgd::uint8_t  hour;
            rgd::uint8_t  minute;
            rgd::uint8_t  second;
        };
        #pragma pack(pop)
    };
    struct model
    {
        png::chunk::ihdr ihdr{};
        png::chunk::plte plte{};
        png::chunk::idat idat{};
        png::chunk::iend iend{};
        png::chunk::trns trns{};
        png::chunk::gama gama{};
        png::chunk::chrm chrm{};
        png::chunk::srgb srgb{};
        png::chunk::iccp iccp{};
        png::chunk::text text{};
        png::chunk::ztxt ztxt{};
        png::chunk::itxt itxt{};
        png::chunk::bkgd bkgd{};
        png::chunk::phys phys{};
        png::chunk::sbit sbit{};
        png::chunk::splt splt{};
        png::chunk::hist hist{};
        png::chunk::time time{};
    };
    struct decode_state
    {
        rgd::bool_t ihdr_reached  = rgd::false_;
        rgd::bool_t iend_reached  = rgd::false_;
        rgd::bool_t idat_previous = rgd::false_;
    };



    auto memory_compare    (std::span<const rgd::byte_t> left, std::span<const rgd::byte_t> right) -> rgd::bool_t
    {
        constexpr auto equal_result = 0;
        return std::memcmp(left.data(), right.data(), std::min(left.size_bytes(), right.size_bytes())) == equal_result;
    }
    auto validate_signature(std::span<const rgd::byte_t> image) -> rgd::bool_t
    {
        return png::memory_compare(image, png::signature);
    }
    auto paeth_predictor   (rgd::byte_t alpha, rgd::byte_t beta, rgd::byte_t gamma) -> rgd::byte_t
        {
            auto const paeth       = alpha + beta - gamma;
            auto const paeth_alpha = std::abs(paeth - alpha);
            auto const paeth_beta  = std::abs(paeth - beta );
            auto const paeth_gamma = std::abs(paeth - gamma);

                 if (paeth_alpha <= paeth_beta  && paeth_alpha <= paeth_gamma) return alpha;
            else if (paeth_beta  <= paeth_gamma                              ) return beta ;
            else                                                               return gamma;
        };
    auto map_image_channels(png::color_type_e color_type) -> std::uint8_t
    {
        switch (color_type)
        {
            case png::color_type_e::grayscale       : return std::uint8_t{ 1u };
            case png::color_type_e::true_color      : return std::uint8_t{ 3u };
            case png::color_type_e::indexed_color   : return std::uint8_t{ 1u };
            case png::color_type_e::grayscale_alpha : return std::uint8_t{ 2u };
            case png::color_type_e::true_color_alpha: return std::uint8_t{ 4u };

            default: throw std::invalid_argument{ "invalid color type" };
        }
    }
    auto validate_ihdr     (png::chunk::ihdr const& ihdr) -> rgd::bool_t
    {
        auto const valid_color_types  = std::array<rgd::uint32_t, 5u>{ 0u, 2u, 3u, 4u, 6u };
        auto const valid_combinations = std::unordered_map<png::color_type_e, std::initializer_list<rgd::uint32_t>>
        {
            { png::color_type_e::grayscale       , { 1u, 2u, 4u, 8u, 16u } }, 
            { png::color_type_e::true_color      , {             8u, 16u } }, 
            { png::color_type_e::indexed_color   , { 1u, 2u, 4u, 8u      } }, 
            { png::color_type_e::grayscale_alpha , {             8u, 16u } }, 
            { png::color_type_e::true_color_alpha, {             8u, 16u } }, 
        };

        if (ihdr.width  == 0u || ihdr.width  == std::numeric_limits<rgd::uint32_t>::max())                  return rgd::false_;
        if (ihdr.height == 0u || ihdr.height == std::numeric_limits<rgd::uint32_t>::max())                  return rgd::false_;
        if (!bit::is_power_of_2(std::to_underlying(ihdr.bit_depth)))                                        return rgd::false_;
        if (!std::ranges::contains(valid_color_types, std::to_underlying(ihdr.color_type)))                 return rgd::false_;
        if (!valid_combinations.contains(ihdr.color_type))                                                  return rgd::false_;
        auto const combination = valid_combinations.at(ihdr.color_type);
        if (!std::ranges::contains(combination, std::to_underlying(ihdr.bit_depth)))                        return rgd::false_;
        if (ihdr.compression_method != png::compression_method_e::deflate)                                  return rgd::false_;
        if (ihdr.filter_method      != png::filter_method_e::adaptive)                                      return rgd::false_;
        if (std::to_underlying(ihdr.interlace_method) > std::to_underlying(png::interlace_method_e::adam7)) return rgd::false_;

        return rgd::true_;
    }
    auto decode            (std::span<const rgd::byte_t> image) -> rgd::image
    {
        if (!png::validate_signature(image)) throw std::invalid_argument{ "invalid png signature" };



        auto decode_state  = png::decode_state{};
        auto cursor        = rgd::cursor_t{};
        auto model         = png::model{};
        
        cursor += sizeof(png::signature);
        while (cursor < image.size_bytes())
        {
            auto chunk_header = png::chunk_header
            {
                .length = png::consume<rgd::uint32_t>(image, cursor), 
                .type   = png::consume<png::chunk_e >(image, cursor), 
            };

            switch (chunk_header.type)
            {
                case png::chunk_e::ihdr:
                {
                    model.ihdr = png::chunk::ihdr
                    {
                        .width              = png::consume<rgd::uint32_t            >(image, cursor), 
                        .height             = png::consume<rgd::uint32_t            >(image, cursor), 
                        .bit_depth          = png::consume<png::bit_depth_e         >(image, cursor), 
                        .color_type         = png::consume<png::color_type_e        >(image, cursor), 
                        .compression_method = png::consume<png::compression_method_e>(image, cursor), 
                        .filter_method      = png::consume<png::filter_method_e     >(image, cursor), 
                        .interlace_method   = png::consume<png::interlace_method_e  >(image, cursor), 
                    };

                    auto const valid_ihdr = png::validate_ihdr(model.ihdr);
                    if (!valid_ihdr) throw;

                    break;
                }
                case png::chunk_e::plte:
                {
                    if (chunk_header.length % sizeof(png::chunk::plte::entry) != 0u) throw std::invalid_argument{ "palette length must be divisible by size of entry!" };
                    
                    auto const entry_count = rgd::size_t{ chunk_header.length / sizeof(png::chunk::plte::entry) };
                    model.plte.entries.resize(entry_count);

                    std::ranges::for_each(std::views::iota(0u, entry_count), [&](auto index)
                        {
                            auto const entry = png::chunk::plte::entry
                            {
                                .red   = png::consume<rgd::uint8_t>(image, cursor), 
                                .green = png::consume<rgd::uint8_t>(image, cursor), 
                                .blue  = png::consume<rgd::uint8_t>(image, cursor), 
                            };

                            model.plte.entries.at(index) = std::move(entry);
                        });

                    break;
                }
                case png::chunk_e::idat:
                {
                    auto const previous_size = model.idat.data.size();
                    auto const new_size      = model.idat.data.size() + chunk_header.length;
                    model.idat.data.resize(new_size);
                    std::memcpy(
                        model.idat.data.data() + previous_size                                               , 
                        image.data() + cursor                                                                ,
                        std::min(static_cast<rgd::size_t>(chunk_header.length), image.size_bytes() - cursor));

                    cursor += chunk_header.length;

                    break;
                }
                case png::chunk_e::iend:
                {
                    decode_state.iend_reached = rgd::true_;

                    break;
                }

                case png::chunk_e::trns: 
                {
                    switch (model.ihdr.color_type)
                    {
                        case png::color_type_e::grayscale      :
                        {
                            model.trns.value = png::chunk::trns::grayscale
                            { 
                                .level = png::consume<rgd::uint16_t>(image, cursor), 
                            };

                            break;
                        }
                        case png::color_type_e::true_color      :
                        {
                            model.trns.value = png::chunk::trns::true_color
                            {
                                .red   = png::consume<rgd::uint16_t>(image, cursor), 
                                .green = png::consume<rgd::uint16_t>(image, cursor), 
                                .blue  = png::consume<rgd::uint16_t>(image, cursor), 
                            };

                            break;
                        }
                        case png::color_type_e::indexed_color  :
                        {
                            auto indexed_color = png::chunk::trns::indexed_color{};
                            std::ranges::for_each(std::views::iota(0u, model.plte.entries.size()), [&](auto)
                                {
                                    indexed_color.entries.emplace_back(png::consume<rgd::uint8_t>(image, cursor));
                                });

                            model.trns.value = std::move(indexed_color);
                            break;
                        }
                    }

                    break;
                }
                case png::chunk_e::gama: 
                {
                    model.gama = png::chunk::gama
                    { 
                        .gamma = png::consume<rgd::uint32_t>(image, cursor), 
                    };

                    break;
                }
                case png::chunk_e::chrm:
                {
                    model.chrm = png::chunk::chrm
                    {
                        .white_point_x = png::consume<std::uint32_t>(image, cursor), 
                        .white_point_y = png::consume<std::uint32_t>(image, cursor), 
                        .red_x         = png::consume<std::uint32_t>(image, cursor), 
                        .red_y         = png::consume<std::uint32_t>(image, cursor), 
                        .green_x       = png::consume<std::uint32_t>(image, cursor), 
                        .green_y       = png::consume<std::uint32_t>(image, cursor), 
                        .blue_x        = png::consume<std::uint32_t>(image, cursor), 
                        .blue_y        = png::consume<std::uint32_t>(image, cursor), 
                    };
                        
                    break;
                }
                case png::chunk_e::srgb:
                {
                    model.srgb = png::chunk::srgb
                    {
                        .rendering_intent = png::consume<png::rendering_intent_e>(image, cursor),
                    };

                    break;
                }
                case png::chunk_e::iccp:
                {
                    auto const profile_name       = std::string{ reinterpret_cast<rgd::char_t const*>(image.data() + cursor) };
                    cursor += profile_name.length() + 1u;
                    auto const compression_method = png::consume<png::compression_method_e>(image, cursor);
                    auto const compressed_profile = std::string{ reinterpret_cast<rgd::char_t const*>(image.data() + cursor) };
                    cursor += compressed_profile.length() + 1u;

                    model.iccp = png::chunk::iccp
                    {
                        .profile_name       = std::move(profile_name)      , 
                        .compression_method = compression_method           , 
                        .compressed_profile = std::move(compressed_profile), 
                    };

                    break;
                }
                case png::chunk_e::text:
                {
                    auto const text           = png::consume<std::string>(image, cursor, chunk_header.length);
                    auto const null_separator = text.find('\0');
                    auto const key            = text.substr(0u, null_separator);
                    auto const value          = text.substr(null_separator + 1u, chunk_header.length - key.length() - 1u);
                    model.text.map.emplace(std::move(key), std::move(value));

                    break;
                }
                case png::chunk_e::ztxt:
                {
                    auto const text               = png::consume<std::string>(image, cursor, chunk_header.length);
                    auto const null_separator     = text.find('\0');
                    auto const key                = text.substr(0u, null_separator);
                    auto const value              = text.substr(null_separator + 1u, chunk_header.length - key.length() - 1u);
                    auto const deflate            = zlib::inflate(bit::as_bytes(value));
                    
                    model.ztxt.map.emplace(std::move(key), std::string{ deflate.begin(), deflate.end() });
                    model.ztxt.compression_method = png::consume<png::compression_method_e>(image, cursor);

                    break;
                }
                case png::chunk_e::itxt:
                {
                    cursor += chunk_header.length;

                    break;
                }
                case png::chunk_e::bkgd: 
                {
                    switch (model.ihdr.color_type)
                    {
                        case png::color_type_e::grayscale       :
                        case png::color_type_e::grayscale_alpha :
                        {
                            model.bkgd.color = png::chunk::bkgd::grayscale
                            { 
                                .level = png::consume<rgd::uint16_t>(image, cursor) 
                            };

                            break;
                        }
                        case png::color_type_e::true_color      :
                        case png::color_type_e::true_color_alpha:
                        {
                            model.bkgd.color = png::chunk::bkgd::true_color
                            {
                                .red   = png::consume<rgd::uint16_t>(image, cursor), 
                                .green = png::consume<rgd::uint16_t>(image, cursor), 
                                .blue  = png::consume<rgd::uint16_t>(image, cursor), 
                            };

                            break;
                        }
                        case png::color_type_e::indexed_color   :
                        {
                            auto indexed_color = png::chunk::bkgd::indexed_color{};
                            std::ranges::for_each(std::views::iota(0u, model.plte.entries.size()), [&](auto)
                                {
                                    indexed_color.entries.emplace_back(png::consume<rgd::uint8_t>(image, cursor));
                                });

                            model.bkgd.color = std::move(indexed_color);
                            break;
                        }
                    }

                    break;
                }
                case png::chunk_e::phys:
                {
                    model.phys = png::chunk::phys
                    {
                        .pixels_per_unit_x = png::consume<rgd::uint32_t        >(image, cursor), 
                        .pixels_per_unit_y = png::consume<rgd::uint32_t        >(image, cursor), 
                        .unit_specifier    = png::consume<png::unit_specifier_e>(image, cursor), 
                    };

                    break;
                }
                case png::chunk_e::sbit: 
                {
                    switch (model.ihdr.color_type)
                    {
                        case png::color_type_e::grayscale       :
                        {
                            model.sbit.color = png::chunk::sbit::grayscale
                            { 
                                .grayscale_significant_bits = png::consume<rgd::uint8_t>(image, cursor) 
                            };

                            break;
                        }
                        case png::color_type_e::true_color      :
                        {
                            model.sbit.color = png::chunk::sbit::true_color
                            {
                                .red_significant_bits   = png::consume<rgd::uint8_t>(image, cursor), 
                                .green_significant_bits = png::consume<rgd::uint8_t>(image, cursor), 
                                .blue_significant_bits  = png::consume<rgd::uint8_t>(image, cursor), 
                            };

                            break;
                        }
                        case png::color_type_e::indexed_color   :
                        {
                            model.sbit.color = png::chunk::sbit::indexed_color
                            { 
                                .red_significant_bits   = png::consume<rgd::uint8_t>(image, cursor), 
                                .green_significant_bits = png::consume<rgd::uint8_t>(image, cursor), 
                                .blue_significant_bits  = png::consume<rgd::uint8_t>(image, cursor), 
                            };

                            break;
                        }
                        case png::color_type_e::grayscale_alpha :
                        {
                            model.sbit.color = png::chunk::sbit::grayscale_alpha
                            {
                                .grayscale_significant_bits = png::consume<rgd::uint8_t>(image, cursor), 
                                .alpha_significant_bits     = png::consume<rgd::uint8_t>(image, cursor), 
                            };

                            break;
                        }
                        case png::color_type_e::true_color_alpha:
                        {
                            model.sbit.color = png::chunk::sbit::true_color_alpha
                            {
                                .red_significant_bits   = png::consume<rgd::uint8_t>(image, cursor), 
                                .green_significant_bits = png::consume<rgd::uint8_t>(image, cursor), 
                                .blue_significant_bits  = png::consume<rgd::uint8_t>(image, cursor), 
                                .alpha_significant_bits = png::consume<rgd::uint8_t>(image, cursor), 
                            };

                            break;
                        }
                    }

                    break;
                }
                case png::chunk_e::hist: 
                {
                    if (chunk_header.length % 2u != 0u) throw std::runtime_error{ "invalid histogram chunk length" };

                    std::ranges::for_each(std::views::iota(0u, chunk_header.length / 2u), [&](auto)
                        {
                            model.hist.data.emplace_back(png::consume<rgd::uint16_t>(image, cursor));
                        });

                    break;
                }
                case png::chunk_e::time:
                {
                    model.time = png::chunk::time
                    {
                        .year   = png::consume<rgd::uint16_t>(image, cursor),
                        .month  = png::consume<rgd::uint8_t >(image, cursor), 
                        .day    = png::consume<rgd::uint8_t >(image, cursor), 
                        .hour   = png::consume<rgd::uint8_t >(image, cursor), 
                        .minute = png::consume<rgd::uint8_t >(image, cursor), 
                        .second = png::consume<rgd::uint8_t >(image, cursor), 
                    };

                    break;
                }

                default:
                {
                    auto const ancillary_bit    = static_cast<png::ancillary_bit_e   >(rgd::bit::test(std::to_underlying(chunk_header.type),  5u));
                    auto const private_bit      = static_cast<png::private_bit_e     >(rgd::bit::test(std::to_underlying(chunk_header.type), 13u));
                    auto const reserved_bit     = static_cast<png::reserved_bit_e    >(rgd::bit::test(std::to_underlying(chunk_header.type), 21u));
                    auto const safe_to_copy_bit = static_cast<png::safe_to_copy_bit_e>(rgd::bit::test(std::to_underlying(chunk_header.type), 29u));

                    std::println("Unknown chunk type");
                    std::println("Chunk properties:");
                    std::println("\t{:16}: {}", "Ancillary bit"   , std::to_underlying(ancillary_bit   ));
                    std::println("\t{:16}: {}", "Private bit"     , std::to_underlying(private_bit     ));
                    std::println("\t{:16}: {}", "Reserved bit"    , std::to_underlying(reserved_bit    ));
                    std::println("\t{:16}: {}", "Safe To Copy bit", std::to_underlying(safe_to_copy_bit));
                }
            }

            chunk_header.crc  = png::consume<rgd::uint32_t>(image, cursor);
        };



        auto const image_channels     = png::map_image_channels(model.ihdr.color_type);
        auto const scanline_size      = rgd::size_t{ model.ihdr.width * image_channels + 1u };
        auto const scanline_data_size = rgd::size_t{ scanline_size                     - 1u };

        auto const inflated_data      = zlib::inflate(model.idat.data);
        auto const total_image_size   = scanline_data_size * model.ihdr.height;
        auto       input_image        = std::vector<rgd::byte_t>(total_image_size);
        auto       output_image       = std::vector<rgd::byte_t>(total_image_size);

        auto       current_operation  = std::function<void(rgd::size_t, rgd::uint8_t, std::span<const rgd::byte_t>, std::span<rgd::byte_t>, std::span<const rgd::byte_t>)>{};
        auto       none_operation     = [](auto column_index, auto               , auto input, auto output, auto      )
            {
                output[column_index] = input[column_index];
            };
        auto       subtract_operation = [](auto column_index, auto image_channels, auto input, auto output, auto      )
            {
                auto const left = column_index >= image_channels ? output[column_index - image_channels] : 0u;
                output[column_index] = input[column_index] + left;
            };
        auto       up_operation       = [](auto column_index, auto               , auto input, auto output, auto prior)
            {
                auto const up = column_index < prior.size() ? prior[column_index] : 0u;
                output[column_index] = input[column_index] + up;
            };
        auto       average_operation  = [](auto column_index, auto image_channels, auto input, auto output, auto prior)
            {
                auto const left    = column_index >= image_channels ? output[column_index - image_channels] : 0u;
                auto const up      = column_index <  prior.size()   ? prior [column_index                 ] : 0u;
                auto const average = (left + up) / 2u;
                        
                output[column_index] = input[column_index] + average;
            };
        auto       paeth_operation    = [](auto column_index, auto image_channels, auto input, auto output, auto prior)
            {
                auto const left    = (column_index >= image_channels                               ) ? output[column_index - image_channels] : 0u;
                auto const up      = (column_index <  prior.size()                                 ) ? prior [column_index                 ] : 0u;
                auto const up_left = (column_index >= image_channels && column_index < prior.size()) ? prior [column_index - image_channels] : 0u;
                auto const paeth   = png::paeth_predictor(left, up, up_left);

                output[column_index] = input[column_index] + paeth;
            };

        std::ranges::for_each(std::views::iota(0u, model.ihdr.height), [&](auto const row_index)
            {
                auto const scanline_start = scanline_size * row_index;
                auto const scanline       = std::span{ inflated_data.data() + scanline_start, scanline_size };
                auto const filter         = static_cast<png::filter_e>(scanline[0u]);

                std::ranges::copy(scanline.subspan(1u), input_image.begin() + (row_index * scanline_data_size));

                auto const input          = std::span<const rgd::byte_t>{ input_image .data() + ( row_index * scanline_data_size), scanline_data_size };
                auto const prior          = (row_index > 0u) ? std::span<const rgd::byte_t>{ output_image.data() + ((row_index - 1) * scanline_data_size), scanline_data_size } : input;
                auto       output         = std::span<      rgd::byte_t>{ output_image.data() + ( row_index * scanline_data_size), scanline_data_size };

                switch (filter)
                {
                    case png::filter_e::none    : current_operation = none_operation    ; break;
                    case png::filter_e::subtract: current_operation = subtract_operation; break;
                    case png::filter_e::up      : current_operation = up_operation      ; break;
                    case png::filter_e::average : current_operation = average_operation ; break;
                    case png::filter_e::paeth   : current_operation = paeth_operation   ; break;
                }
                std::ranges::for_each(std::views::iota(0u, input.size()), [&](auto index)
                    {
                        current_operation(index, image_channels, input, output, prior);
                    });
            });



        return rgd::image
        { 
            .layout     = rgd::image::layout_e::rgb                          , 
            .dimensions = rgd::vector_2u{model.ihdr.width, model.ihdr.height}, 
            .data       = output_image                                       , 
        };
    }
}
