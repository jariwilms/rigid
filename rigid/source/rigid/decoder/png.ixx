export module rigid.decode.png;

import std;
import rigid.core;
import zlib_ng;

namespace rgd::mem::adapter
{
    //A specialized allocator that bypasses initialization when a vector is resized.
    //This allows for direct population of the raw, uninitialized memory.
    //It can significantly improve performance for large allocations by eliminating redundant initialization.
    //https://stackoverflow.com/a/21028912
    //https://www.boost.org/doc/libs/1_75_0/boost/core/noinit_adaptor.hpp
    //template<typename A>
    //class no_init : public A
    //{
    //public:
    //    template<typename U> 
    //    struct rebind
    //    {
    //        using other = no_init<typename std::allocator_traits<A>::template rebind_alloc<U>>;
    //    };

    //    template<typename U>
    //    void construct(U* pointer) noexcept(std::is_nothrow_default_constructible<U>::value)
    //    {
    //        ::new(static_cast<void*>(pointer)) U;
    //    }
    //    template<typename U, typename...Args>
    //    void construct(U* pointer, Args&&... args)
    //    {
    //        std::allocator_traits<A>::construct(static_cast<A&>(*this), pointer, std::forward<Args>(args)...);
    //    }

    //    template<class U>
    //    void destroy(U* pointer)
    //    {
    //        pointer->~U();
    //    }

    //    template<class T, class U>
    //    friend auto operator==(const no_init<T>& left, const no_init<U>& right) noexcept -> rgd::bool_t
    //    {
    //        return static_cast<const T&>(left) == static_cast<const U&>(right);
    //    }
    //    template<class T, class U>
    //    friend auto operator!=(const no_init<T>& left, const no_init<U>& right) noexcept -> rgd::bool_t
    //    {
    //        return !(left == right);
    //    }
    //};

    template<typename T>
    class no_init {
        static_assert(
            std::is_fundamental<T>::value,
            "should be a fundamental type");
    public:
        no_init() noexcept {}
        constexpr  no_init(T value) noexcept : v_{ value } {}
        constexpr  operator T () const noexcept { return v_; }
    private:
        T v_;
    };
}
export namespace rgd
{
    enum class error_type_e
    {
        runtime         , 
        overflow        , 
        invalid_argument, 
        out_of_range    , 
    };
    struct error_result
    {
        rgd::error_type_e type;
        std::string_view  message;
    };
}
export namespace rgd::png
{
    enum class accessibility_e : rgd::uint8_t
    {
        public_  = 0u, 
        private_ = 1u, 
    };
    enum class bit_depth_e : rgd::uint8_t
    {
        _1  =  1u, 
        _2  =  2u, 
        _4  =  4u, 
        _8  =  8u, 
        _16 = 16u, 
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
            png::bit_depth_e          bit_depth;
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
    auto map_color_channels      (png::pixel_type_e pixel_type) -> rgd::size_t
    {
        switch (pixel_type)
        {
            using enum png::pixel_type_e;

            case grayscale       : return 1u;
            case true_color      : return 3u;
            case indexed_color   : return 1u;
            case grayscale_alpha : return 2u;
            case true_color_alpha: return 4u;

            default: throw std::invalid_argument{ "invalid color type" };
        }
    }
    auto calculate_scanline_size (rgd::size_t size, png::bit_depth_e bit_depth) -> rgd::size_t
    {
        auto const factor = std::to_underlying(bit_depth) / 8.0f;
        return static_cast<rgd::size_t>(std::ceil(size * factor));
    }
    auto paeth_predictor         (rgd::byte_t alpha, rgd::byte_t beta, rgd::byte_t gamma) -> rgd::byte_t
    {
        auto threshold = gamma * 3 - (alpha + beta);
        auto low       = alpha < beta ? alpha : beta;
        auto high      = alpha < beta ? beta  : alpha;
        
        auto t0        = (high <= threshold) ? low  : gamma;
        auto t1        = (threshold <= low ) ? high : t0;
        
        return t1;
    };
    
    template<rgd::size_t SourceChannels, rgd::size_t TargetChannels>
    auto convert_channels        (rgd::vector_2u const& dimensions, std::span<const rgd::byte_t> memory) -> std::vector<rgd::byte_t>
    {
        auto const pixels = rgd::size_t{ dimensions.x * dimensions.y };
        auto       target = std::vector<rgd::byte_t>(pixels * TargetChannels);

        for (auto i = rgd::size_t{ 0u }; i < pixels; ++i)
        {
            auto const* s = &memory[i * SourceChannels];
            auto      * d = &target[i * TargetChannels];

            for (auto j = rgd::size_t{ 0u }; j < TargetChannels; ++j)
            {
                     if (j < SourceChannels) d[j] = s[j];
                else if (j < 3u)             d[j] = (SourceChannels == 1u) ? s[0u] : 0u;
                else                         d[j] = 255u;
            }
        }

        return target;
    }
    auto convert_layout          (rgd::vector_2u const& dimensions, rgd::image_layout_e source_layout, rgd::image_layout_e target_layout, std::span<const rgd::byte_t> memory) -> std::vector<rgd::byte_t>
    {
        switch (source_layout)
        {
            using enum rgd::image_layout_e;

            case r   : 
            {
                switch (target_layout)
                {
                    using enum rgd::image_layout_e;

                    case r   : return png::convert_channels<1u, 1u>(dimensions, memory);
                    case rg  : return png::convert_channels<1u, 2u>(dimensions, memory);
                    case rgb : return png::convert_channels<1u, 3u>(dimensions, memory);
                    case rgba: return png::convert_channels<1u, 4u>(dimensions, memory);

                    default: throw std::invalid_argument{ "invalid layout" };
                }
            }
            case rg  : 
            {
                switch (target_layout)
                {
                    using enum rgd::image_layout_e;

                    case r   : return png::convert_channels<2u, 1u>(dimensions, memory);
                    case rg  : return png::convert_channels<2u, 2u>(dimensions, memory);
                    case rgb : return png::convert_channels<2u, 3u>(dimensions, memory);
                    case rgba: return png::convert_channels<2u, 4u>(dimensions, memory);

                    default: throw std::invalid_argument{ "invalid layout" };
                }
            }
            case rgb : 
            {
                switch (target_layout)
                {
                    using enum rgd::image_layout_e;

                    case r   : return png::convert_channels<3u, 1u>(dimensions, memory);
                    case rg  : return png::convert_channels<3u, 2u>(dimensions, memory);
                    case rgb : return png::convert_channels<3u, 3u>(dimensions, memory);
                    case rgba: return png::convert_channels<3u, 4u>(dimensions, memory);

                    default: throw std::invalid_argument{ "invalid layout" };
                }
            }
            case rgba: 
            {
                switch (target_layout)
                {
                    using enum rgd::image_layout_e;

                    case r   : return png::convert_channels<4u, 1u>(dimensions, memory);
                    case rg  : return png::convert_channels<4u, 2u>(dimensions, memory);
                    case rgb : return png::convert_channels<4u, 3u>(dimensions, memory);
                    case rgba: return png::convert_channels<4u, 4u>(dimensions, memory);

                    default: throw std::invalid_argument{ "invalid layout" };
                }
            }

            default: throw std::invalid_argument{ "invalid layout" };
        }
    }
    auto expand_bits             (rgd::vector_2u const& dimensions, png::bit_depth_e bit_depth, std::span<const rgd::byte_t> memory) -> std::vector<rgd::byte_t>
    {
        auto const bits_per_byte    = std::to_underlying(bit_depth);
        auto const samples_per_byte = 8u / bits_per_byte;
        auto const bytes_per_row    = (dimensions.x + samples_per_byte - 1) / samples_per_byte;
        
        auto const value_lut        = std::invoke([&]() -> std::array<rgd::byte_t, 16u>
            {
                auto const maximum      = (1u << bits_per_byte) - 1u;
                auto const scale_factor = 255u / ((1u << bits_per_byte) - 1u);
                auto       lut          = std::array<rgd::byte_t, 16u>{};
                
                for (auto i = rgd::size_t{ 0u }; i <= maximum; ++i)
                {
                    lut[i] = static_cast<rgd::byte_t>(i * scale_factor);
                }

                return lut;
            });

        auto const mask             = (1u << bits_per_byte) - 1u;
        auto       output           = std::vector<rgd::byte_t>(dimensions.x * dimensions.y);

        for (auto y = rgd::size_t{ 0u }; y < dimensions.y; ++y)
        {
            auto const index_in  = y * bytes_per_row;
            auto const index_out = y * dimensions.x;

            for (auto x = rgd::size_t{ 0u }; x < dimensions.x; ++x)
            {
                auto const byte_index = x / samples_per_byte;
                auto const bit_index  = x % samples_per_byte;
                auto const shift      = (samples_per_byte - 1u - bit_index) * bits_per_byte;
                auto const sample     = static_cast<rgd::byte_t>((memory[index_in + byte_index] >> shift) & mask);

                output[index_out + x] = value_lut[sample];
            }
        }

        return output;
    }

    auto validate_signature      (std::span<const rgd::byte_t> memory) -> rgd::bool_t
    {
        return mem::compare(memory, png::signature);
    }
    auto parse_chunks            (std::span<const rgd::byte_t> memory) -> png::model
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
                        .bit_depth          = stream.consume<png::bit_depth_e         >(), 
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
                        memory     .data()     + stream.position()                                                       , 
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

                        default: throw std::invalid_argument{ "invalid pixel type" };
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
            }

            chunk_header.crc = stream.consume<rgd::uint32_t>();
        };

        return model;
    }
    auto unfilter_scanlines      (rgd::vector_2u const& dimensions, rgd::image_layout_e layout, png::bit_depth_e bit_depth, std::span<const rgd::byte_t> memory) -> std::vector<rgd::byte_t>
    {
        auto const bytes_per_pixel   = rgd::size_t{ (std::to_underlying(bit_depth) * std::to_underlying(layout) + 7u) / 8u };
        auto const scanline_size     = png::calculate_scanline_size(dimensions.x * bytes_per_pixel, bit_depth);
        auto const scanline_stride   = rgd::size_t{ scanline_size + 1u };

        auto       output_buffer     = std::vector<rgd::byte_t>(scanline_size * dimensions.y);
        auto       input_scanline    = std::span<const rgd::byte_t>{};
        auto       output_scanline   = std::span<rgd::byte_t>{};
        auto       previous_scanline = std::span<rgd::byte_t>{};

        for (auto row_index = rgd::size_t{ 0u }; row_index < dimensions.y; ++row_index)
        {
            auto const row_scanline          = memory.subspan(row_index * scanline_stride, scanline_stride);
            auto const row_filter            = static_cast<png::filter_type_e>(row_scanline[0u]);
            auto const has_previous_scanline = !previous_scanline.empty();
            
            input_scanline                   = row_scanline.subspan(1u, scanline_size);
            output_scanline                  = std::span<rgd::byte_t>{ output_buffer.data() + row_index * scanline_size, scanline_size };

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
                    std::ranges::copy(input_scanline.subspan(0u, bytes_per_pixel), output_scanline.begin());
                    for (auto index = bytes_per_pixel; index < scanline_size; ++index)
                    {
                        output_scanline[index] = input_scanline[index] + output_scanline[index - bytes_per_pixel];
                    }

                    break;
                }
                case up      :
                {
                    if (has_previous_scanline)
                    {
                        for (auto index = rgd::size_t{ 0u }; index < scanline_size; ++index)
                        {
                            output_scanline[index] = input_scanline[index] + previous_scanline[index];
                        }
                    }
                    else
                    {
                        std::ranges::copy(input_scanline, output_scanline.begin());
                    }

                    break;
                }
                case average :
                {
                    for (auto index = rgd::size_t{ 0u }; index < bytes_per_pixel; ++index)
                    {
                        auto const up          = has_previous_scanline ? previous_scanline[index] : rgd::byte_t{ 0u };
                        auto const average     = static_cast<rgd::byte_t>(up / 2u);
                        output_scanline[index] = input_scanline[index] + average;
                    }
                    for (auto index = bytes_per_pixel; index < scanline_size; ++index)
                    {
                        auto const left        = output_scanline[index - bytes_per_pixel];
                        auto const up          = has_previous_scanline ? previous_scanline[index] : rgd::byte_t{ 0u };
                        auto const average     = static_cast<rgd::byte_t>((static_cast<rgd::uint32_t>(left) + up) / 2u);
                        output_scanline[index] = input_scanline[index] + average;
                    }

                    break;
                }
                case paeth   :
                {
                    for (auto index = rgd::size_t{ 0u }; index < bytes_per_pixel; ++index)
                    {
                        auto const up          = has_previous_scanline ? previous_scanline[index] : rgd::byte_t{ 0u };
                        auto const predictor   = png::paeth_predictor(rgd::byte_t{ 0u }, up, rgd::byte_t{ 0u });
                        output_scanline[index] = input_scanline[index] + predictor;
                    }
                    for (auto index = bytes_per_pixel; index < scanline_size; ++index)
                    {
                        auto const left        = output_scanline[index - bytes_per_pixel];
                        auto const up          = has_previous_scanline ? previous_scanline[index                  ] : rgd::byte_t{ 0u };
                        auto const up_left     = has_previous_scanline ? previous_scanline[index - bytes_per_pixel] : rgd::byte_t{ 0u };
                        auto const predictor   = png::paeth_predictor(left, up, up_left);
                        output_scanline[index] = input_scanline[index] + predictor;
                    }

                    break;
                }

                default: throw std::invalid_argument{ "invalid filter type" };
            }

            previous_scanline                = output_scanline;
        }

        return output_buffer;
    }
    auto unfilter_scanlines_adam7(rgd::vector_2u const& dimensions, rgd::image_layout_e layout, png::bit_depth_e bit_depth, std::span<const rgd::byte_t> memory) -> std::vector<rgd::byte_t>
    {
        auto const output_buffer_size = dimensions.x * dimensions.y * std::to_underlying(layout);
        auto       output_buffer      = std::vector<rgd::byte_t>(output_buffer_size);
        auto       bytes_processed    = rgd::size_t{ 0u };

        for (auto const& pass : png::adam7_pass_data)
        {
            auto const pass_dimensions = rgd::vector_2u
            {
                (dimensions.x - pass.offset.x + pass.stride.x - 1u) / pass.stride.x,
                (dimensions.y - pass.offset.y + pass.stride.y - 1u) / pass.stride.y
            };
            auto const pass_data       = memory.subspan(bytes_processed);
            auto const pass_image      = png::unfilter_scanlines(pass_dimensions, layout, bit_depth, pass_data);
            if (pass_dimensions.x == 0u || pass_dimensions.y == 0u) continue;

            for (auto row_index = rgd::size_t{ 0u }; row_index < pass_dimensions.y; ++row_index)
            {
                for (auto column_index = rgd::size_t{ 0u }; column_index < pass_dimensions.x; ++column_index)
                {
                    auto const source_pixel_offset     = (row_index * pass_dimensions.x + column_index) * std::to_underlying(layout);
                    auto const destination_row         = pass.offset.y + row_index     * pass.stride.y;
                    auto const destination_column      = pass.offset.x + column_index  * pass.stride.x;
                    auto const destination_pixel_index = destination_row               * dimensions.x + destination_column;
                    auto const destination_byte_offset = destination_pixel_index       * std::to_underlying(layout);
                            
                    std::memcpy(output_buffer.data() + destination_byte_offset, pass_image.data() + source_pixel_offset, std::to_underlying(layout));
                }
            }

            bytes_processed += (pass_dimensions.x * pass_dimensions.y * std::to_underlying(layout)) + pass_dimensions.y;
        }

        return output_buffer;
    }

    auto decode                  (std::optional<rgd::image_layout_e> target_layout, std::span<const rgd::byte_t> memory) -> rgd::image
    {
        //1. Validate Signature : Ensure the data is a PNG file
        //2. Stream Parsing     : Validate file and create image model
        //3. Decompression      : Inflate the data stream
        //4. Reconstruction     : Reverse the byte-level filtering
        //5. Pixel normalization: Normalize sub-byte channels to full bytes.
        //6. Layout conversion  : Convert image to requested layout

        auto const signature_span   = memory.subspan<0u, png::signature.size()>();
        auto const valid_signature  = png::validate_signature(memory);
        if (!valid_signature) throw;

        auto const model_span       = memory.subspan<png::signature.size()>();
        auto const model            = png::parse_chunks(model_span);
        auto const color_channels   = png::map_color_channels(model.ihdr.pixel_type);
        auto const layout           = static_cast<rgd::image_layout_e>(color_channels);
        
        auto       result_data      = zng::inflate(model.idat.data);

        if (model.ihdr.interlace_method == png::interlace_method_e::none) result_data = png::unfilter_scanlines      (model.ihdr.dimensions, layout, model.ihdr.bit_depth, result_data);
        else                                                              result_data = png::unfilter_scanlines_adam7(model.ihdr.dimensions, layout, model.ihdr.bit_depth, result_data);
                                                          
        if (model.ihdr.bit_depth != png::bit_depth_e::_8)                 result_data = png::expand_bits             (model.ihdr.dimensions,         model.ihdr.bit_depth , result_data);
        if (target_layout.has_value() && target_layout.value() != layout) result_data = png::convert_layout          (model.ihdr.dimensions, layout, target_layout.value(), result_data);

        return rgd::image
        {
            .layout     = target_layout.value_or(layout), 
            .dimensions = model.ihdr.dimensions         , 
            .data       = std::move(result_data)        , 
        };
    }
}
