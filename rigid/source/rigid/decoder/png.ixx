export module rigid.decode.png;

import std;
import rigid.core;
import zlib_ng;

export namespace rgd
{
    template<rgd::image_layout_e input_layout, rgd::image_layout_e output_layout>
    void convert_layout(rgd::vector_2u dimensions, std::span<const rgd::byte_t> input, std::span<rgd::byte_t> output)
    {
        auto constexpr input_channels  = std::to_underlying(input_layout);
        auto constexpr output_channels = std::to_underlying(output_layout);
        auto const     pixel_count     = dimensions.x * dimensions.y;

        if constexpr (output_channels > input_channels)
        {
            for (auto index = pixel_count; index-- > 0u;)
            {
                auto const input_index  = index * input_channels;
                auto const output_index = index * output_channels;

                for (auto channel_index =       0u; channel_index < input_channels       ; ++channel_index)
                {
                    output[output_index + channel_index] = input[input_index + channel_index];
                }
                for (auto channel_index = input_channels; channel_index < output_channels; ++channel_index)
                {
                    output[output_index + channel_index] = 0u;
                }
                if constexpr (output_channels == 4 && input_channels < 4)
                {
                    output[output_index + 3u] = 255u;
                }
            }
        }
        else
        {
            auto const copy_count = std::min(input_channels, output_channels);

            for (auto index = 0u; index < pixel_count; ++index)
            {
                auto const input_index  = index * input_channels;
                auto const output_index = index * output_channels;

                for (auto channel_index = 0u; channel_index < copy_count; ++channel_index)
                {
                    output[output_index + channel_index] = input[input_index + channel_index];
                }
            }
        }
    }
    void convert_layout(rgd::vector_2u dimensions, rgd::image_layout_e input_layout, rgd::image_layout_e output_layout, std::span<const rgd::byte_t> input, std::span<rgd::byte_t> output)
    {
        auto invoke = [&]<rgd::image_layout_e input_layout>(rgd::image_layout_e output_layout) -> void
        {
            if (input_layout == output_layout) return;

            switch (output_layout)
            {
                case rgd::image_layout_e::r   : return convert_layout<input_layout, rgd::image_layout_e::r   >(dimensions, input, output);
                case rgd::image_layout_e::rg  : return convert_layout<input_layout, rgd::image_layout_e::rg  >(dimensions, input, output);
                case rgd::image_layout_e::rgb : return convert_layout<input_layout, rgd::image_layout_e::rgb >(dimensions, input, output);
                case rgd::image_layout_e::rgba: return convert_layout<input_layout, rgd::image_layout_e::rgba>(dimensions, input, output);

                default: throw std::invalid_argument{ "invalid layout" };
            }
        };

        switch (input_layout)
        {
            case rgd::image_layout_e::r   : return invoke.template operator()<rgd::image_layout_e::r   >(output_layout);
            case rgd::image_layout_e::rg  : return invoke.template operator()<rgd::image_layout_e::rg  >(output_layout);
            case rgd::image_layout_e::rgb : return invoke.template operator()<rgd::image_layout_e::rgb >(output_layout);
            case rgd::image_layout_e::rgba: return invoke.template operator()<rgd::image_layout_e::rgba>(output_layout);
            
            default: throw std::invalid_argument{ "invalid layout" };
        }
    }

    template<rgd::uint8_t bit_depth>
    auto constexpr make_scale_table()
    {
        auto constexpr enumerations = (1u << bit_depth) - 1u;
        auto           table        = std::array<rgd::byte_t, enumerations + 1u>{};

        for (auto index = rgd::size_t{ 0u }; index <= enumerations; ++index)
        {
            table[index] = static_cast<rgd::uint8_t>((index * 255u) / enumerations);
        }

        return table;
    }
    template<rgd::uint8_t bit_depth>
    void expand_bits(rgd::size_t pixel_count, std::span<rgd::byte_t> input, std::span<rgd::byte_t> output)
    {
        if constexpr (bit_depth != rgd::uint8_t{ 16u })
        {
            auto constexpr pixels_per_byte = 8u / bit_depth;
            auto constexpr mask            = rgd::byte_t{ (1u << bit_depth) - 1u };
            auto constexpr scale_table     = rgd::make_scale_table<bit_depth>();

            for (auto index = pixel_count - 1u; index < std::numeric_limits<rgd::size_t>::max(); --index)
            {
                auto const byte_index = index / pixels_per_byte;
                auto const bit_index  = index % pixels_per_byte;
                auto const shift      = (pixels_per_byte - 1u - bit_index) * bit_depth;
                auto const sample     = (input[byte_index] >> shift) & mask;

                output[index]         = scale_table[sample];
            }
        }
        else
        {
            for (auto index = rgd::size_t{ 0u }; index < pixel_count; ++index)
            {
                auto const high_byte = input[index * 2u];
                output[index]        = high_byte;
            }
        }
    }
}
export namespace rgd::png
{
    using bit_depth   = rgd::uint8_t;
    using bit_depth_t = rgd::uint8_t;
    using channels_t  = rgd::uint8_t;

    enum class accessibility_e : rgd::uint8_t
    {
        public_  = 0u, 
        private_ = 1u, 
    };
    enum class chunk_type_e : rgd::uint32_t
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
    enum class chunk_requirement_e : rgd::uint8_t
    {
        critical  = 0u, 
        ancillary = 1u, 
    };
    enum class compression_flag_e : rgd::uint8_t
    {
        uncompressed = 0u, 
        compressed   = 1u, 
    };
    enum class compression_method_e : rgd::uint8_t
    {
        deflate = 0u, 
    };
    enum class filter_method_e : rgd::uint8_t
    {
        adaptive = 0u, 
    };
    enum class filter_type_e : rgd::uint8_t
    {
        none     = 0u, 
        subtract = 1u, 
        up       = 2u, 
        average  = 3u, 
        paeth    = 4u, 
    };
    enum class interlace_method_e : rgd::uint8_t
    {
        none  = 0u, 
        adam7 = 1u, 
    };
    enum class pixel_type_e : rgd::uint8_t
    {
        grayscale        = 0u                          , 
        true_color       = 2u                          , 
        indexed_color    = true_color | 1u             , 
        grayscale_alpha  = 4u                          , 
        true_color_alpha = true_color | grayscale_alpha, 
    };
    enum class rendering_intent_e : rgd::uint8_t
    {
        perceptual            = 0u, 
        relative_colorimetric = 1u, 
        saturation            = 2u, 
        absolute_colorimetric = 3u, 
    };
    enum class reserved_e : rgd::uint8_t
    {
        _ = 0u, 
    };
    enum class safe_to_copy_e : rgd::uint8_t
    {
        no  = 0u, 
        yes = 1u, 
    };
    enum class unit_specifier_e : rgd::uint8_t
    {
        unknown = 0u, 
        meter   = 1u, 
    };

    struct chunk
    {
        struct header
        {
            rgd::uint32_t     length;
            png::chunk_type_e type;
            rgd::uint32_t     crc;
        };

        struct ihdr
        {
            rgd::vector_2u            dimensions;
            png::bit_depth            bit_depth;
            png::pixel_type_e         pixel_type;
            png::compression_method_e compression_method;
            png::filter_method_e      filter_method;
            png::interlace_method_e   interlace_method;
        };
        struct plte
        {
            struct entry
            {
                rgd::uint8_t red;
                rgd::uint8_t green;
                rgd::uint8_t blue;
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
        struct trns
        {
            struct grayscale
            {
                rgd::uint16_t level;
            };
            struct true_color
            {
                rgd::uint16_t red;
                rgd::uint16_t green;
                rgd::uint16_t blue;
            };
            struct indexed_color
            {
                std::vector<rgd::uint8_t> entries;
            };

            std::variant<grayscale, true_color, indexed_color> value;
        };
        struct gama
        {
            rgd::uint32_t gamma;
        };
        struct chrm
        {
            rgd::vector_2u white_point;
            rgd::vector_2u red;
            rgd::vector_2u green;
            rgd::vector_2u blue;
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
                rgd::uint16_t level;
            };
            struct true_color
            {
                rgd::uint16_t red;
                rgd::uint16_t green;
                rgd::uint16_t blue;
            };
            struct indexed_color
            {
                std::vector<rgd::uint8_t> entries;
            };

            std::variant<grayscale, true_color, indexed_color> color;
        };
        struct phys
        {
            rgd::vector_2u        pixels_per_unit;
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
            //TODO
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
    struct decoding_state
    {
        rgd::bool_t ihdr_reached  = rgd::false_;
        rgd::bool_t iend_reached  = rgd::false_;
        rgd::bool_t idat_previous = rgd::false_;
    };
    struct adam7_pass
    {
        rgd::vector_2u offset;
        rgd::vector_2u stride;
    };
}
export namespace rgd::png
{
    auto constexpr signature       = std::array<rgd::byte_t, 8u>
    {
        0x89,
        0x50,
        0x4E,
        0x47,
        0x0D,
        0x0A,
        0x1A,
        0x0A,
    };
    auto constexpr adam7_pass_data = std::array<png::adam7_pass, 7u>
    {
        png::adam7_pass{.offset = { 0u, 0u }, .stride = { 8u, 8u } },
        png::adam7_pass{.offset = { 4u, 0u }, .stride = { 8u, 8u } },
        png::adam7_pass{.offset = { 0u, 4u }, .stride = { 4u, 8u } },
        png::adam7_pass{.offset = { 2u, 0u }, .stride = { 4u, 4u } },
        png::adam7_pass{.offset = { 0u, 2u }, .stride = { 2u, 4u } },
        png::adam7_pass{.offset = { 1u, 0u }, .stride = { 2u, 2u } },
        png::adam7_pass{.offset = { 0u, 1u }, .stride = { 1u, 2u } },
    };
}
export namespace rgd::png
{
    auto map_image_layout          (png::pixel_type_e pixel_type) -> rgd::image_layout_e
    {
        switch (pixel_type)
        {
            using enum png::pixel_type_e;

            case grayscale       : return rgd::image_layout_e::r   ;
            case true_color      : return rgd::image_layout_e::rgb ;
            case indexed_color   : return rgd::image_layout_e::r   ;
            case grayscale_alpha : return rgd::image_layout_e::rg  ;
            case true_color_alpha: return rgd::image_layout_e::rgba;

            default: throw std::invalid_argument{ "invalid pixel type" };
        }
    }
    
    auto calculate_pixel_size      (rgd::image_layout_e image_layout, png::bit_depth bit_depth) -> rgd::size_t
    {
        return (std::to_underlying(image_layout) * bit_depth + 7u) / 8u;
    }
    auto calculate_scanline_size   (rgd::size_t pixels, png::bit_depth bit_depth) -> rgd::size_t
    {
        auto const factor = bit_depth / 8.0f;
        return static_cast<rgd::size_t>(std::ceil(pixels * factor));
    }
    auto calculate_filter_size     (png::bit_depth bit_depth) -> rgd::size_t
    {
        return (bit_depth == 16u) ? 2u : 1u;
    }
    auto calculate_total_image_size(rgd::vector_2u const& dimensions, rgd::image_layout_e image_layout, png::bit_depth bit_depth) -> rgd::size_t
    {
        auto const multiplier = (bit_depth == 16u) ? 2u : 1u;
        return dimensions.x * dimensions.y * std::to_underlying(image_layout) * multiplier;
    }
    auto calculate_pass_dimensions (rgd::vector_2u const& dimensions, png::adam7_pass const& pass) -> rgd::vector_2u
    {
        return rgd::vector_2u
        {
            (dimensions.x - pass.offset.x + pass.stride.x - 1u) / pass.stride.x, 
            (dimensions.y - pass.offset.y + pass.stride.y - 1u) / pass.stride.y, 
        };
    }
    
    auto paeth_predictor           (rgd::byte_t alpha, rgd::byte_t beta, rgd::byte_t gamma) -> rgd::byte_t
    {
        auto const threshold = gamma * 3 - (alpha + beta);
        auto const low       = alpha < beta ? alpha : beta;
        auto const high      = alpha < beta ? beta  : alpha;
        
        auto const t0        = (high <= threshold) ? low  : gamma;
        auto const t1        = (threshold <= low ) ? high : t0;
        
        return t1;
    };
    auto print_chunk_info          (png::chunk::header chunk_header)
    {
        auto const ancillary_bit    = static_cast<png::chunk_requirement_e>(rgd::bit::test(std::to_underlying(chunk_header.type),  5u));
        auto const private_bit      = static_cast<png::accessibility_e    >(rgd::bit::test(std::to_underlying(chunk_header.type), 13u));
        auto const reserved_bit     = static_cast<png::reserved_e         >(rgd::bit::test(std::to_underlying(chunk_header.type), 21u));
        auto const safe_to_copy_bit = static_cast<png::safe_to_copy_e     >(rgd::bit::test(std::to_underlying(chunk_header.type), 29u));

        std::println("Unknown chunk type");
        std::println("Chunk properties:");
        std::println("\t{:16}: {}", "Ancillary bit"   , std::to_underlying(ancillary_bit   ));
        std::println("\t{:16}: {}", "Private bit"     , std::to_underlying(private_bit     ));
        std::println("\t{:16}: {}", "Reserved bit"    , std::to_underlying(reserved_bit    ));
        std::println("\t{:16}: {}", "Safe To Copy bit", std::to_underlying(safe_to_copy_bit));
    }

    auto validate_signature        (std::span<const rgd::byte_t> memory) -> rgd::bool_t
    {
        return mem::compare(memory, png::signature);
    }
    auto parse_chunks              (std::span<const rgd::byte_t> memory) -> png::model
    {
        auto model          = png::model{};
        auto decoding_state = png::decoding_state{};
        auto stream         = rgd::input_stream<bit::endian_e::big>{ memory };

        while (stream.position() < memory.size_bytes())
        {
            auto chunk_header = png::chunk::header
            {
                .length = stream.consume<rgd::uint32_t    >(), 
                .type   = stream.consume<png::chunk_type_e>(), 
            };

            switch (chunk_header.type)
            {
                case png::chunk_type_e::ihdr:
                {
                    model.ihdr = png::chunk::ihdr
                    {
                        .dimensions = rgd::vector_2u
                        {
                            stream.consume<rgd::uint32_t>(), 
                            stream.consume<rgd::uint32_t>(), 
                        }, 
                        .bit_depth          = stream.consume<png::bit_depth           >(), 
                        .pixel_type         = stream.consume<png::pixel_type_e        >(), 
                        .compression_method = stream.consume<png::compression_method_e>(), 
                        .filter_method      = stream.consume<png::filter_method_e     >(), 
                        .interlace_method   = stream.consume<png::interlace_method_e  >(), 
                    };

                    break;
                }
                case png::chunk_type_e::plte:
                {
                    auto const entry_count = rgd::size_t{ chunk_header.length / sizeof(png::chunk::plte::entry) };
                    model.plte.entries.resize(entry_count);

                    for (auto& entry : model.plte.entries)
                    {
                        entry = png::chunk::plte::entry
                        {
                            .red   = stream.consume<rgd::uint8_t>(), 
                            .green = stream.consume<rgd::uint8_t>(), 
                            .blue  = stream.consume<rgd::uint8_t>(), 
                        };
                    }

                    break;
                }
                case png::chunk_type_e::idat:
                {
                    auto const previous_size = model.idat.data.size();
                    auto const new_size      = model.idat.data.size() + chunk_header.length;
                    model.idat.data.resize(new_size);

                    std::memcpy(
                        model.idat.data.data() + previous_size                                                           , 
                        memory         .data() + stream.position()                                                       , 
                        std::min(static_cast<rgd::size_t>(chunk_header.length), memory.size_bytes() - stream.position()));

                    stream.forward(chunk_header.length);

                    break;
                }
                case png::chunk_type_e::iend:
                {
                    decoding_state.iend_reached = rgd::true_;

                    break;
                }
                
                case png::chunk_type_e::trns: 
                {
                    switch (model.ihdr.pixel_type)
                    {
                        case png::pixel_type_e::grayscale      :
                        {
                            model.trns.value = png::chunk::trns::grayscale
                            { 
                                .level = stream.consume<rgd::uint16_t>(),
                            };

                            break;
                        }
                        case png::pixel_type_e::true_color      :
                        {
                            model.trns.value = png::chunk::trns::true_color
                            {
                                .red   = stream.consume<rgd::uint16_t>(), 
                                .green = stream.consume<rgd::uint16_t>(), 
                                .blue  = stream.consume<rgd::uint16_t>(), 
                            };

                            break;
                        }
                        case png::pixel_type_e::indexed_color  :
                        {
                            auto indexed_color = png::chunk::trns::indexed_color{};
                            for (auto index = rgd::size_t{ 0u }; index < model.plte.entries.size(); ++index)
                            {
                                indexed_color.entries.emplace_back(stream.consume<rgd::uint8_t>());
                            }

                            model.trns.value = std::move(indexed_color);
                            break;
                        }
                    }

                    break;
                }
                case png::chunk_type_e::gama: 
                {
                    model.gama = png::chunk::gama
                    { 
                        .gamma = stream.consume<rgd::uint32_t>(),
                    };

                    break;
                }
                case png::chunk_type_e::chrm:
                {
                    model.chrm = png::chunk::chrm
                    {
                        .white_point = rgd::vector_2u{ stream.consume<std::uint32_t>(), stream.consume<std::uint32_t>() }, 
                        .red         = rgd::vector_2u{ stream.consume<std::uint32_t>(), stream.consume<std::uint32_t>() }, 
                        .green       = rgd::vector_2u{ stream.consume<std::uint32_t>(), stream.consume<std::uint32_t>() }, 
                        .blue        = rgd::vector_2u{ stream.consume<std::uint32_t>(), stream.consume<std::uint32_t>() }, 
                    };
                        
                    break;
                }
                case png::chunk_type_e::srgb:
                {
                    model.srgb = png::chunk::srgb
                    {
                        .rendering_intent = stream.consume<png::rendering_intent_e>(),
                    };

                    break;
                }
                case png::chunk_type_e::iccp:
                {
                    //auto const profile_name       = std::string{ reinterpret_cast<rgd::char_t const*>(memory.data() + cursor) };
                    //cursor += profile_name.length() + 1u;
                    //auto const compression_method = png::consume<png::compression_method_e>(memory, cursor);
                    //auto const compressed_profile = std::string{ reinterpret_cast<rgd::char_t const*>(memory.data() + cursor) };
                    //cursor += compressed_profile.length() + 1u;

                    //model.iccp = png::chunk::iccp
                    //{
                    //    .profile_name       = std::move(profile_name)      , 
                    //    .compression_method = compression_method           , 
                    //    .compressed_profile = std::move(compressed_profile), 
                    //};

                    stream.forward(chunk_header.length);
                    break;
                }
                case png::chunk_type_e::text:
                {
                    //auto const text           = png::consume<std::string>(memory, cursor, chunk_header.length);
                    //auto const null_separator = text.find('\0');
                    //auto const key            = text.substr(0u, null_separator);
                    //auto const value          = text.substr(null_separator + 1u, chunk_header.length - key.length() - 1u);
                    //model.text.map.emplace(std::move(key), std::move(value));

                    stream.forward(chunk_header.length);
                    break;
                }
                case png::chunk_type_e::ztxt:
                {
                    //auto const text               = png::consume<std::string>(memory, cursor, chunk_header.length);
                    //auto const null_separator     = text.find('\0');
                    //auto const key                = text.substr(0u, null_separator);
                    //auto const value              = text.substr(null_separator + 1u, chunk_header.length - key.length() - 1u);
                    //auto const deflate            = zng::inflate(bit::as_bytes(value));
                    //
                    //model.ztxt.map.emplace(std::move(key), std::string{ deflate.begin(), deflate.end() });
                    //model.ztxt.compression_method = png::consume<png::compression_method_e>(memory, cursor);

                    stream.forward(chunk_header.length);
                    break;
                }
                case png::chunk_type_e::itxt:
                {
                    stream.forward(chunk_header.length);
                    break;
                }
                case png::chunk_type_e::bkgd: 
                {
                    switch (model.ihdr.pixel_type)
                    {
                        case png::pixel_type_e::grayscale       :
                        case png::pixel_type_e::grayscale_alpha :
                        {
                            model.bkgd.color = png::chunk::bkgd::grayscale
                            { 
                                .level = stream.consume<rgd::uint16_t>()
                            };

                            break;
                        }
                        case png::pixel_type_e::true_color      :
                        case png::pixel_type_e::true_color_alpha:
                        {
                            model.bkgd.color = png::chunk::bkgd::true_color
                            {
                                .red   = stream.consume<rgd::uint16_t>(), 
                                .green = stream.consume<rgd::uint16_t>(), 
                                .blue  = stream.consume<rgd::uint16_t>(), 
                            };

                            break;
                        }
                        case png::pixel_type_e::indexed_color   :
                        {
                            auto indexed_color = png::chunk::bkgd::indexed_color{};
                            for (auto index = rgd::size_t{ 0u }; index < model.plte.entries.size(); ++index)
                            {
                                indexed_color.entries.emplace_back(stream.consume<rgd::uint8_t>());
                            }

                            model.bkgd.color = std::move(indexed_color);
                            break;
                        }

                        default: std::unreachable();
                    }

                    break;
                }
                case png::chunk_type_e::phys:
                {
                    model.phys = png::chunk::phys
                    {
                        .pixels_per_unit = rgd::vector_2u{ stream.consume<rgd::uint32_t>(), stream.consume<rgd::uint32_t>() }, 
                        .unit_specifier  = stream.consume<png::unit_specifier_e>()                                           , 
                    };

                    break;
                }
                case png::chunk_type_e::sbit: 
                {
                    switch (model.ihdr.pixel_type)
                    {
                        case png::pixel_type_e::grayscale       :
                        {
                            model.sbit.color = png::chunk::sbit::grayscale
                            { 
                                .grayscale_significant_bits = stream.consume<rgd::uint8_t>()
                            };

                            break;
                        }
                        case png::pixel_type_e::true_color      :
                        {
                            model.sbit.color = png::chunk::sbit::true_color
                            {
                                .red_significant_bits   = stream.consume<rgd::uint8_t>(), 
                                .green_significant_bits = stream.consume<rgd::uint8_t>(), 
                                .blue_significant_bits  = stream.consume<rgd::uint8_t>(), 
                            };

                            break;
                        }
                        case png::pixel_type_e::indexed_color   :
                        {
                            model.sbit.color = png::chunk::sbit::indexed_color
                            { 
                                .red_significant_bits   = stream.consume<rgd::uint8_t>(), 
                                .green_significant_bits = stream.consume<rgd::uint8_t>(), 
                                .blue_significant_bits  = stream.consume<rgd::uint8_t>(), 
                            };

                            break;
                        }
                        case png::pixel_type_e::grayscale_alpha :
                        {
                            model.sbit.color = png::chunk::sbit::grayscale_alpha
                            {
                                .grayscale_significant_bits = stream.consume<rgd::uint8_t>(), 
                                .alpha_significant_bits     = stream.consume<rgd::uint8_t>(), 
                            };

                            break;
                        }
                        case png::pixel_type_e::true_color_alpha:
                        {
                            model.sbit.color = png::chunk::sbit::true_color_alpha
                            {
                                .red_significant_bits   = stream.consume<rgd::uint8_t>(), 
                                .green_significant_bits = stream.consume<rgd::uint8_t>(), 
                                .blue_significant_bits  = stream.consume<rgd::uint8_t>(), 
                                .alpha_significant_bits = stream.consume<rgd::uint8_t>(), 
                            };

                            break;
                        }
                    }

                    break;
                }
                case png::chunk_type_e::hist: 
                {
                    for (auto index = rgd::size_t{ 0u }; index < chunk_header.length / 2u; ++index)
                    {
                        model.hist.data.emplace_back(stream.consume<rgd::uint16_t>());
                    }

                    break;
                }
                case png::chunk_type_e::time:
                {
                    model.time = png::chunk::time
                    {
                        .year   = stream.consume<rgd::uint16_t>(), 
                        .month  = stream.consume<rgd::uint8_t >(), 
                        .day    = stream.consume<rgd::uint8_t >(), 
                        .hour   = stream.consume<rgd::uint8_t >(), 
                        .minute = stream.consume<rgd::uint8_t >(), 
                        .second = stream.consume<rgd::uint8_t >(), 
                    };

                    break;
                }

                default:
                {
                    stream.forward(chunk_header.length);
                }
            }

            chunk_header.crc = stream.consume<rgd::uint32_t>();
        };

        return model;
    }
    void unfilter_scanlines        (rgd::vector_2u const& dimensions, rgd::image_layout_e image_layout, png::bit_depth bit_depth, std::span<rgd::byte_t> memory)
    {
        auto const pixel_size        = png::calculate_pixel_size   (image_layout, bit_depth);
        auto const scanline_size     = png::calculate_scanline_size(dimensions.x * pixel_size, bit_depth);
        auto const filter_size       = png::calculate_filter_size  (bit_depth);
        auto const scanline_stride   = scanline_size + filter_size;

        auto       input_scanline    = std::span<const rgd::byte_t>{};
        auto       output_scanline   = std::span<rgd::byte_t>{};
        auto       previous_scanline = std::span<rgd::byte_t>{};

        {
            auto const row_filter = static_cast<png::filter_type_e>(memory[0u]);
            
            input_scanline        = std::span<rgd::byte_t>{ memory.data() + filter_size, scanline_size };
            output_scanline       = std::span<rgd::byte_t>{ memory.data()              , scanline_size };

            switch (row_filter)
            {
                using enum png::filter_type_e;
            
                case none    :
                {
                    std::ranges::copy(input_scanline, output_scanline.begin());

                    break;
                }
                case subtract:
                {
                    std::ranges::copy(input_scanline.subspan(0u, pixel_size), output_scanline.begin());
                    for (auto index = pixel_size; index < scanline_size; ++index)
                    {
                        output_scanline[index] = input_scanline[index] + output_scanline[index - pixel_size];
                    }
                
                    break;
                }
                case up      :
                case average :
                case paeth   :
                {
                    for (auto index = rgd::size_t{ 0u }; index < pixel_size; ++index)
                    {
                        output_scanline[index] = input_scanline[index];
                    }
                    for (auto index = pixel_size; index < scanline_size; ++index)
                    {
                        output_scanline[index] = input_scanline[index] + output_scanline[index - pixel_size];
                    }
                
                    break;
                }
            
                default: throw std::invalid_argument{ "invalid filter" };
            }
        }
        for (auto row_index = rgd::size_t{ 1u }; row_index < dimensions.y; ++row_index)
        {
            auto const row_filter = static_cast<png::filter_type_e>(memory[row_index * scanline_stride]);

            previous_scanline     = output_scanline;
            input_scanline        = std::span<rgd::byte_t>{ memory.data() + row_index * scanline_stride + filter_size, scanline_size };
            output_scanline       = std::span<rgd::byte_t>{ memory.data() + row_index * scanline_size                , scanline_size };

            switch (row_filter)
            {
                using enum png::filter_type_e;

                case none    :
                {
                    std::ranges::copy(input_scanline, output_scanline.begin());

                    break;
                }
                case subtract:
                {
                    std::ranges::copy(input_scanline.subspan(0u, pixel_size), output_scanline.begin());
                    for (auto index = pixel_size; index < scanline_size; ++index)
                    {
                        output_scanline[index] = input_scanline[index] + output_scanline[index - pixel_size];
                    }

                    break;
                }
                case up      :
                {
                    for (auto index = rgd::size_t{ 0u }; index < scanline_size; ++index)
                    {
                        output_scanline[index] = input_scanline[index] + previous_scanline[index];
                    }

                    break;
                }
                case average :
                {
                    for (auto index = rgd::size_t{ 0u }; index < pixel_size; ++index)
                    {
                        auto const average     = static_cast<rgd::byte_t>(previous_scanline[index] / 2u);
                        output_scanline[index] = input_scanline[index] + average;
                    }
                    for (auto index = pixel_size; index < scanline_size; ++index)
                    {
                        auto const left        = output_scanline  [index - pixel_size];
                        auto const up          = previous_scanline[index                  ];
                        auto const average     = rgd::byte_t{ (left + up) / 2u };
                        output_scanline[index] = input_scanline   [index                  ] + average;
                    }

                    break;
                }
                case paeth   :
                {
                    for (auto index = rgd::size_t{ 0u }; index < pixel_size; ++index)
                    {
                        auto const predictor   = png::paeth_predictor(rgd::byte_t{ 0u }, previous_scanline[index], rgd::byte_t{ 0u });
                        output_scanline[index] = input_scanline[index] + predictor;
                    }
                    for (auto index = pixel_size; index < scanline_size; ++index)
                    {
                        auto const left        = output_scanline  [index - pixel_size];
                        auto const up          = previous_scanline[index                  ];
                        auto const up_left     = previous_scanline[index - pixel_size];
                        auto const predictor   = png::paeth_predictor(left, up, up_left);
                        output_scanline[index] = input_scanline   [index                  ] + predictor;
                    }

                    break;
                }

                default: throw std::invalid_argument{ "invalid filter" };
            }
        }
    }
    void unfilter_scanlines_adam7  (rgd::vector_2u const& dimensions, rgd::image_layout_e image_layout, png::bit_depth bit_depth, std::span<rgd::byte_t> memory)
    {
        auto const pixel_size   = png::calculate_pixel_size (image_layout, bit_depth);
        auto const filter_size  = png::calculate_filter_size(bit_depth);
        auto const output_size  = static_cast<size_t>(dimensions.y) * dimensions.x * pixel_size;
        auto       output       = std::vector<rgd::byte_t>(output_size);
        auto       input_offset = rgd::size_t{ 0u };

        for (auto const& pass : png::adam7_pass_data)
        {
            auto const pass_dimensions = png::calculate_pass_dimensions(dimensions, pass);
            if (pass_dimensions.x == 0u || pass_dimensions.y == 0u) continue;

            auto const scanline_size   = pass_dimensions.x * pixel_size;
            auto const scanline_stride = scanline_size + filter_size;
            auto const pass_input_size = pass_dimensions.y * scanline_stride;
            auto       pass_data       = std::vector<rgd::byte_t>(pass_input_size);
            auto       target_position = rgd::vector_2u{};

            std::memcpy            (pass_data.data(), memory.data() + input_offset, pass_input_size);
            png::unfilter_scanlines(pass_dimensions, image_layout, bit_depth, pass_data);
            input_offset += pass_input_size;

            for (auto y = rgd::size_t{ 0u }; y < pass_dimensions.y; ++y)
            {
                target_position.y               = static_cast<rgd::uint32_t>(pass.offset.y + y * pass.stride.y);
                auto const pass_scanline_offset = y * scanline_size;

                for (auto x = rgd::size_t{ 0u }; x < pass_dimensions.x; ++x)
                {
                    target_position.x             = static_cast<rgd::uint32_t>(pass.offset.x + x * pass.stride.x);
                    auto const output_offset      = (target_position.y * dimensions.x + target_position.x) * pixel_size;
                    auto const input_pixel_offset = pass_scanline_offset + x * pixel_size;

                    std::memcpy(output.data() + output_offset, pass_data.data() + input_pixel_offset, pixel_size);
                }
            }
        }

        std::memcpy(memory.data(), output.data(), output.size());
    }

    void flip_image_vertical       (rgd::vector_2u const& dimensions, rgd::image_layout_e layout, std::span<rgd::byte_t> memory)
    {
        auto const channels = std::to_underlying(layout);
        auto const row_size = dimensions.x * channels;

        for (auto row_index = rgd::size_t{ 0u }; row_index < dimensions.y / 2u; ++row_index)
        {
            auto const top_index    = row_index * row_size;
            auto const bottom_index = (dimensions.y - 1u - row_index) * row_size;

            std::ranges::swap_ranges(memory.subspan(top_index, row_size), memory.subspan(bottom_index, row_size));
        }
    }

    auto decode                    (std::optional<rgd::image_layout_e> target_layout, std::span<const rgd::byte_t> memory) -> rgd::image
    {
        //1. Validate Signature : Ensure the data is a PNG file
        //2. Chunk Parsing      : Validate file and create image model
        //3. Decompression      : Inflate the data stream
        //4. Reconstruction     : Reverse the byte-level filtering
        //5. Post Processing    : Mirror, expand, and/or convert the image
        
        auto const signature_span    = memory.subspan<0u, png::signature.size()>();
        auto const valid_signature   = png::validate_signature(memory);
        if (!valid_signature) throw std::invalid_argument{ "invalid png signature" };
        
        auto const model             = png::parse_chunks(memory.subspan<png::signature.size()>());
        auto const layout            = png::map_image_layout(model.ihdr.pixel_type);
        target_layout                = target_layout.value_or(layout);
        
        auto       data              = zng::inflate(model.idat.data.size(), model.idat.data);
        auto const maximum_size      = png::calculate_total_image_size(model.ihdr.dimensions, std::max(layout, target_layout.value()), model.ihdr.bit_depth);
        data.resize(maximum_size);
        
        auto const is_interlaced     = model.ihdr.interlace_method == png::interlace_method_e::adam7;
        if   (is_interlaced) png::unfilter_scanlines_adam7(model.ihdr.dimensions, layout, model.ihdr.bit_depth, data);
        else                 png::unfilter_scanlines      (model.ihdr.dimensions, layout, model.ihdr.bit_depth, data);
        
        if (model.ihdr.bit_depth  !=     8u) rgd::expand_bits<1u>(model.ihdr.dimensions.x * model.ihdr.dimensions.y, data, data);
        if (target_layout.value() != layout) rgd::convert_layout(model.ihdr.dimensions, layout, target_layout.value(), data, data);
        //if (rgd::true_                     ) png::flip_image_vertical(model.ihdr.dimensions, layout, data);
        
        auto const final_size        = model.ihdr.dimensions.x * model.ihdr.dimensions.y * std::to_underlying(target_layout.value());
        data.resize(final_size);
        
        return rgd::image
        {
            .layout     = target_layout.value(), 
            .dimensions = model.ihdr.dimensions, 
            .data       = std::move(data)      , 
        };
    }
}
