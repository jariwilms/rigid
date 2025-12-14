export module rigid.decode.png;

import std;
import zlib_ng;
import rigid.core;

export namespace rgd::png
{
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
    struct adam7_pass
    {
        rgd::vector_2u offset;
        rgd::vector_2u stride;
    };

    using decode_function           = rgd::void_t(*)(std::span<const rgd::byte_t>, std::span<const rgd::byte_t>, std::span<rgd::byte_t>, rgd::size_t, rgd::uint8_t);
    auto constexpr signature        = std::array<rgd::byte_t    , 8u>
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
    auto constexpr adam7_image_data = std::array<png::adam7_pass, 7u>
    {
        png::adam7_pass{ .offset = { 0u, 0u }, .stride = { 8u, 8u } }, 
        png::adam7_pass{ .offset = { 4u, 0u }, .stride = { 8u, 8u } }, 
        png::adam7_pass{ .offset = { 0u, 4u }, .stride = { 4u, 8u } }, 
        png::adam7_pass{ .offset = { 2u, 0u }, .stride = { 4u, 4u } }, 
        png::adam7_pass{ .offset = { 0u, 2u }, .stride = { 2u, 4u } }, 
        png::adam7_pass{ .offset = { 1u, 0u }, .stride = { 2u, 2u } }, 
        png::adam7_pass{ .offset = { 0u, 1u }, .stride = { 1u, 2u } }, 
    };



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



    auto memory_compare    (std::span<const rgd::byte_t> left, std::span<const rgd::byte_t> right) -> rgd::bool_t
    {
        auto constexpr equal_result = 0;
        return std::memcmp(left.data(), right.data(), std::min(left.size_bytes(), right.size_bytes())) == equal_result;
    }
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
    auto none_operation    (std::span<const rgd::byte_t> input, std::span<const rgd::byte_t>         , std::span<rgd::byte_t> output, rgd::size_t column_index, rgd::uint8_t               )
    {
        output[column_index] = input[column_index];
    }
    auto subtract_operation(std::span<const rgd::byte_t> input, std::span<const rgd::byte_t>         , std::span<rgd::byte_t> output, rgd::size_t column_index, rgd::uint8_t image_channels)
    {
        auto const left      = column_index >= image_channels ? output[column_index - image_channels] : rgd::byte_t{ 0u };
        output[column_index] = input[column_index] + left;
    }
    auto up_operation      (std::span<const rgd::byte_t> input, std::span<const rgd::byte_t> previous, std::span<rgd::byte_t> output, rgd::size_t column_index, rgd::uint8_t               )
    {
        auto const up        = !previous.empty() ? previous[column_index] : rgd::byte_t{ 0u };
        output[column_index] = input[column_index] + up;
    }
    auto average_operation (std::span<const rgd::byte_t> input, std::span<const rgd::byte_t> previous, std::span<rgd::byte_t> output, rgd::size_t column_index, rgd::uint8_t image_channels)
    {
        auto const left      = column_index >= image_channels ? output  [column_index - image_channels] : rgd::byte_t{ 0u };
        auto const up        = !previous.empty()              ? previous[column_index                 ] : rgd::byte_t{ 0u };
        auto const average   = static_cast<rgd::byte_t>((static_cast<rgd::uint32_t>(left) + up) / 2u);
        output[column_index] = input[column_index] + average;
    }
    auto paeth_operation   (std::span<const rgd::byte_t> input, std::span<const rgd::byte_t> previous, std::span<rgd::byte_t> output, rgd::size_t column_index, rgd::uint8_t image_channels)
    {
        auto const left      = column_index >= image_channels                      ? output  [column_index - image_channels] : rgd::byte_t{ 0u };
        auto const up        = !previous.empty()                                   ? previous[column_index                 ] : rgd::byte_t{ 0u };
        auto const up_left   = column_index >= image_channels && !previous.empty() ? previous[column_index - image_channels] : rgd::byte_t{ 0u };
        auto const predictor = paeth_predictor(left, up, up_left);
        output[column_index] = input[column_index] + predictor;
    }

    auto validate_signature(std::span<const rgd::byte_t> image) -> rgd::bool_t
    {
        return png::memory_compare(image, png::signature);
    }
    auto validate_ihdr     (png::chunk::ihdr const& ihdr) -> rgd::bool_t
    {
        auto const valid_color_types        = std::array<png::color_type_e, 5u>
        {
            png::color_type_e::grayscale       , 
            png::color_type_e::true_color      , 
            png::color_type_e::indexed_color   , 
            png::color_type_e::grayscale_alpha , 
            png::color_type_e::true_color_alpha, 
        };
        auto const valid_color_combinations = std::unordered_map<png::color_type_e, std::initializer_list<rgd::uint32_t>>
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
        if (!std::ranges::contains(valid_color_types, ihdr.color_type))                                     return rgd::false_;
        if (!valid_color_combinations.contains(ihdr.color_type))                                            return rgd::false_;
        auto const combination = valid_color_combinations.at(ihdr.color_type);
        if (!std::ranges::contains(combination, std::to_underlying(ihdr.bit_depth)))                        return rgd::false_;
        if (ihdr.compression_method != png::compression_method_e::deflate)                                  return rgd::false_;
        if (ihdr.filter_method      != png::filter_method_e::adaptive)                                      return rgd::false_;
        if (std::to_underlying(ihdr.interlace_method) > std::to_underlying(png::interlace_method_e::adam7)) return rgd::false_;

        return rgd::true_;
    }
    
    auto parse_chunks      (std::span<const rgd::byte_t> chunk_data) -> png::model
    {
        auto decode_state = png::decode_state{};
        auto cursor       = rgd::cursor_t{};
        auto model        = png::model{};

        while (cursor < chunk_data.size_bytes())
        {
            auto chunk_header = png::chunk_header
            {
                .length = png::consume<rgd::uint32_t>(chunk_data, cursor), 
                .type   = png::consume<png::chunk_e >(chunk_data, cursor), 
            };

            switch (chunk_header.type)
            {
                case png::chunk_e::ihdr:
                {
                    model.ihdr = png::chunk::ihdr
                    {
                        .width              = png::consume<rgd::uint32_t            >(chunk_data, cursor), 
                        .height             = png::consume<rgd::uint32_t            >(chunk_data, cursor), 
                        .bit_depth          = png::consume<png::bit_depth_e         >(chunk_data, cursor), 
                        .color_type         = png::consume<png::color_type_e        >(chunk_data, cursor), 
                        .compression_method = png::consume<png::compression_method_e>(chunk_data, cursor), 
                        .filter_method      = png::consume<png::filter_method_e     >(chunk_data, cursor), 
                        .interlace_method   = png::consume<png::interlace_method_e  >(chunk_data, cursor), 
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

                    for (auto index = rgd::size_t{ 0u }; index < entry_count; ++index)
                    {
                        auto const entry = png::chunk::plte::entry
                        {
                            .red   = png::consume<rgd::uint8_t>(chunk_data, cursor), 
                            .green = png::consume<rgd::uint8_t>(chunk_data, cursor), 
                            .blue  = png::consume<rgd::uint8_t>(chunk_data, cursor), 
                        };

                        model.plte.entries.at(index) = std::move(entry);
                    }

                    break;
                }
                case png::chunk_e::idat:
                {
                    auto const previous_size = model.idat.data.size();
                    auto const new_size      = model.idat.data.size() + chunk_header.length;
                    model.idat.data.resize(new_size);
                    std::memcpy(
                        model.idat.data.data() + previous_size                                                    , 
                        chunk_data     .data() + cursor                                                           , 
                        std::min(static_cast<rgd::size_t>(chunk_header.length), chunk_data.size_bytes() - cursor));

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
                                .level = png::consume<rgd::uint16_t>(chunk_data, cursor), 
                            };

                            break;
                        }
                        case png::color_type_e::true_color      :
                        {
                            model.trns.value = png::chunk::trns::true_color
                            {
                                .red   = png::consume<rgd::uint16_t>(chunk_data, cursor), 
                                .green = png::consume<rgd::uint16_t>(chunk_data, cursor), 
                                .blue  = png::consume<rgd::uint16_t>(chunk_data, cursor), 
                            };

                            break;
                        }
                        case png::color_type_e::indexed_color  :
                        {
                            auto indexed_color = png::chunk::trns::indexed_color{};
                            for (auto index = rgd::size_t{ 0u }; index < model.plte.entries.size(); ++index)
                            {
                                indexed_color.entries.emplace_back(png::consume<rgd::uint8_t>(chunk_data, cursor));
                            }

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
                        .gamma = png::consume<rgd::uint32_t>(chunk_data, cursor), 
                    };

                    break;
                }
                case png::chunk_e::chrm:
                {
                    model.chrm = png::chunk::chrm
                    {
                        .white_point_x = png::consume<std::uint32_t>(chunk_data, cursor), 
                        .white_point_y = png::consume<std::uint32_t>(chunk_data, cursor), 
                        .red_x         = png::consume<std::uint32_t>(chunk_data, cursor), 
                        .red_y         = png::consume<std::uint32_t>(chunk_data, cursor), 
                        .green_x       = png::consume<std::uint32_t>(chunk_data, cursor), 
                        .green_y       = png::consume<std::uint32_t>(chunk_data, cursor), 
                        .blue_x        = png::consume<std::uint32_t>(chunk_data, cursor), 
                        .blue_y        = png::consume<std::uint32_t>(chunk_data, cursor), 
                    };
                        
                    break;
                }
                case png::chunk_e::srgb:
                {
                    model.srgb = png::chunk::srgb
                    {
                        .rendering_intent = png::consume<png::rendering_intent_e>(chunk_data, cursor),
                    };

                    break;
                }
                case png::chunk_e::iccp:
                {
                    auto const profile_name       = std::string{ reinterpret_cast<rgd::char_t const*>(chunk_data.data() + cursor) };
                    cursor += profile_name.length() + 1u;
                    auto const compression_method = png::consume<png::compression_method_e>(chunk_data, cursor);
                    auto const compressed_profile = std::string{ reinterpret_cast<rgd::char_t const*>(chunk_data.data() + cursor) };
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
                    auto const text           = png::consume<std::string>(chunk_data, cursor, chunk_header.length);
                    auto const null_separator = text.find('\0');
                    auto const key            = text.substr(0u, null_separator);
                    auto const value          = text.substr(null_separator + 1u, chunk_header.length - key.length() - 1u);
                    model.text.map.emplace(std::move(key), std::move(value));

                    break;
                }
                case png::chunk_e::ztxt:
                {
                    auto const text               = png::consume<std::string>(chunk_data, cursor, chunk_header.length);
                    auto const null_separator     = text.find('\0');
                    auto const key                = text.substr(0u, null_separator);
                    auto const value              = text.substr(null_separator + 1u, chunk_header.length - key.length() - 1u);
                    auto const deflate            = zng::inflate(bit::as_bytes(value));
                    
                    model.ztxt.map.emplace(std::move(key), std::string{ deflate.begin(), deflate.end() });
                    model.ztxt.compression_method = png::consume<png::compression_method_e>(chunk_data, cursor);

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
                                .level = png::consume<rgd::uint16_t>(chunk_data, cursor) 
                            };

                            break;
                        }
                        case png::color_type_e::true_color      :
                        case png::color_type_e::true_color_alpha:
                        {
                            model.bkgd.color = png::chunk::bkgd::true_color
                            {
                                .red   = png::consume<rgd::uint16_t>(chunk_data, cursor), 
                                .green = png::consume<rgd::uint16_t>(chunk_data, cursor), 
                                .blue  = png::consume<rgd::uint16_t>(chunk_data, cursor), 
                            };

                            break;
                        }
                        case png::color_type_e::indexed_color   :
                        {
                            auto indexed_color = png::chunk::bkgd::indexed_color{};
                            for (auto index = rgd::size_t{ 0u }; index < model.plte.entries.size(); ++index)
                            {
                                indexed_color.entries.emplace_back(png::consume<rgd::uint8_t>(chunk_data, cursor));
                            }

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
                        .pixels_per_unit_x = png::consume<rgd::uint32_t        >(chunk_data, cursor), 
                        .pixels_per_unit_y = png::consume<rgd::uint32_t        >(chunk_data, cursor), 
                        .unit_specifier    = png::consume<png::unit_specifier_e>(chunk_data, cursor), 
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
                                .grayscale_significant_bits = png::consume<rgd::uint8_t>(chunk_data, cursor) 
                            };

                            break;
                        }
                        case png::color_type_e::true_color      :
                        {
                            model.sbit.color = png::chunk::sbit::true_color
                            {
                                .red_significant_bits   = png::consume<rgd::uint8_t>(chunk_data, cursor), 
                                .green_significant_bits = png::consume<rgd::uint8_t>(chunk_data, cursor), 
                                .blue_significant_bits  = png::consume<rgd::uint8_t>(chunk_data, cursor), 
                            };

                            break;
                        }
                        case png::color_type_e::indexed_color   :
                        {
                            model.sbit.color = png::chunk::sbit::indexed_color
                            { 
                                .red_significant_bits   = png::consume<rgd::uint8_t>(chunk_data, cursor), 
                                .green_significant_bits = png::consume<rgd::uint8_t>(chunk_data, cursor), 
                                .blue_significant_bits  = png::consume<rgd::uint8_t>(chunk_data, cursor), 
                            };

                            break;
                        }
                        case png::color_type_e::grayscale_alpha :
                        {
                            model.sbit.color = png::chunk::sbit::grayscale_alpha
                            {
                                .grayscale_significant_bits = png::consume<rgd::uint8_t>(chunk_data, cursor), 
                                .alpha_significant_bits     = png::consume<rgd::uint8_t>(chunk_data, cursor), 
                            };

                            break;
                        }
                        case png::color_type_e::true_color_alpha:
                        {
                            model.sbit.color = png::chunk::sbit::true_color_alpha
                            {
                                .red_significant_bits   = png::consume<rgd::uint8_t>(chunk_data, cursor), 
                                .green_significant_bits = png::consume<rgd::uint8_t>(chunk_data, cursor), 
                                .blue_significant_bits  = png::consume<rgd::uint8_t>(chunk_data, cursor), 
                                .alpha_significant_bits = png::consume<rgd::uint8_t>(chunk_data, cursor), 
                            };

                            break;
                        }
                    }

                    break;
                }
                case png::chunk_e::hist: 
                {
                    if (chunk_header.length % 2u != 0u) throw std::runtime_error{ "invalid histogram chunk length" };

                    for (auto index = rgd::size_t{ 0u }; index < chunk_header.length / 2u; ++index)
                    {
                        model.hist.data.emplace_back(png::consume<rgd::uint16_t>(chunk_data, cursor));
                    }

                    break;
                }
                case png::chunk_e::time:
                {
                    model.time = png::chunk::time
                    {
                        .year   = png::consume<rgd::uint16_t>(chunk_data, cursor),
                        .month  = png::consume<rgd::uint8_t >(chunk_data, cursor), 
                        .day    = png::consume<rgd::uint8_t >(chunk_data, cursor), 
                        .hour   = png::consume<rgd::uint8_t >(chunk_data, cursor), 
                        .minute = png::consume<rgd::uint8_t >(chunk_data, cursor), 
                        .second = png::consume<rgd::uint8_t >(chunk_data, cursor), 
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

            chunk_header.crc  = png::consume<rgd::uint32_t>(chunk_data, cursor);
        };

        return model;
    }
    auto decode_sub_image  (rgd::vector_2u const& dimensions, png::color_type_e color_type, std::span<const rgd::byte_t> image_data) -> rgd::image
    {
        auto const image_channels     = png::map_image_channels(color_type);
        auto const scanline_size      = rgd::size_t{ dimensions.x * image_channels + 1u };
        auto const scanline_data_size = rgd::size_t{ scanline_size                 - 1u };
        auto const total_image_size   = scanline_data_size * dimensions.y;
        auto const decode_operation   = std::unordered_map<png::filter_e, decode_function>
        {
            { png::filter_e::none    , png::none_operation     },
            { png::filter_e::subtract, png::subtract_operation },
            { png::filter_e::up      , png::up_operation       },
            { png::filter_e::average , png::average_operation  },
            { png::filter_e::paeth   , png::paeth_operation    },
        };

        auto       result_image       = std::vector<rgd::byte_t>(total_image_size);
        auto       input_scanline     = std::span<const rgd::byte_t>{};
        auto       previous_scanline  = std::span<const rgd::byte_t>{};
        auto       output_scanline    = std::span<      rgd::byte_t>{};

        for (auto row_index = rgd::size_t{ 0u }; row_index < dimensions.y; ++row_index)
        {
            auto const current_scanline  = std::span{ image_data.data() + row_index * scanline_size, scanline_size };
            auto const filter            = static_cast<png::filter_e>(current_scanline[0u]);
            auto const current_operation = decode_operation.at(filter);
            input_scanline               = current_scanline.subspan<1u>();
            output_scanline              = std::span{ result_image.data() + row_index * scanline_data_size, scanline_data_size };

            for (auto column_index = rgd::size_t{ 0u }; column_index < input_scanline.size(); ++column_index)
            {
                current_operation(input_scanline, previous_scanline, output_scanline, column_index, image_channels);
            }

            previous_scanline            = output_scanline;
        }

        return rgd::image
        { 
            .layout     = rgd::image::layout_e::rgb, 
            .dimensions = dimensions               , 
            .data       = result_image             , 
        };
    }
    
    auto decode            (std::span<const rgd::byte_t> image_data) -> rgd::image
    {
        if (!png::validate_signature(image_data)) throw std::invalid_argument{ "invalid png signature" };

        auto const image_model    = png::parse_chunks(image_data.subspan<sizeof(png::signature)>());
        auto const image_channels = png::map_image_channels(image_model.ihdr.color_type);
        auto const inflated_data  = zng::inflate(image_model.idat.data);
        auto const current_data   = std::span{ inflated_data };
        auto       image          = rgd::image{};
        
        switch (image_model.ihdr.interlace_method)
        {
            case png::interlace_method_e::none : 
            {
                image = png::decode_sub_image(rgd::vector_2u{ image_model.ihdr.width, image_model.ihdr.height }, image_model.ihdr.color_type, inflated_data);
                break;
            }
            case png::interlace_method_e::adam7: 
            {
                auto index        = rgd::size_t{ 0u };
                auto data_offset  = rgd::size_t{ 0u };
                image             = rgd::image
                {
                    .layout     = rgd::image_layout_e::rgb                                                                   , 
                    .dimensions = { image_model.ihdr.width, image_model.ihdr.height }                                        , 
                    .data       = std::vector<rgd::byte_t>(image_model.ihdr.width * image_model.ihdr.height * image_channels), 
                };
                for (auto const& pass : png::adam7_image_data)
                {
                    auto const pass_dimensions = rgd::vector_2u
                    {
                        (image_model.ihdr.width  - pass.offset.x + pass.stride.x - 1u) / pass.stride.x, 
                        (image_model.ihdr.height - pass.offset.y + pass.stride.y - 1u) / pass.stride.y, 
                    };
                    if (std::min(pass_dimensions.x, pass_dimensions.y) > 0u)
                    {
                        auto const pass_data    = current_data.subspan(data_offset);
                        auto const pass_image   = decode_sub_image(pass_dimensions, image_model.ihdr.color_type, pass_data);
                        data_offset            += (pass_dimensions.x * pass_dimensions.y * image_channels) + pass_dimensions.y;

                        for (auto pass_y = 0u; pass_y < pass_dimensions.y; ++pass_y)
                        {
                            for (auto pass_x = 0u; pass_x < pass_dimensions.x; ++pass_x)
                            {
                                auto const result_x      = pass.offset.x + pass_x * pass.stride.x;
                                auto const result_y      = pass.offset.y + pass_y * pass.stride.y;
                                auto const pass_offset   = (pass_y * pass_dimensions.x + pass_x) * image_channels;
                                auto const result_offset = (result_y * image_model.ihdr.width + result_x) * image_channels;

                                std::memcpy(&image.data[result_offset], &pass_image.data[pass_offset], image_channels);
                            }
                        }
                    }
                    
                    ++index;
                }

                break;
            }

            default: throw std::invalid_argument{ "invalid interlace method" };
        }

        return image;
    }
}
