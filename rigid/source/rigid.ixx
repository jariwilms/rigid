export module rigid;
export import rigid.core;
export import rigid.decode.png;
export import rigid.decode.jpeg;
export import rigid.decode.webp;
export import rigid.decode.bmp;

import std;

export namespace rgd
{
    enum class option_e
    {
        no  = rgd::false_, 
        yes = rgd::true_, 
    };
    enum class scan_type_e
    {
        minimal , 
        metadata, 
        complete, 
    };
    enum class bit_depth_e : rgd::uint8_t
    {
        _1  =  1u, 
        _2  =  2u, 
        _4  =  4u, 
        _8  =  8u, 
        _16 = 16u, 
    };
    enum class swizzle_e
    {
        rgba, 
        bgra, 
        argb, 
    };

    struct decode_options
    {
        rgd::scan_type_e    scan_type;
        rgd::option_e       validate;
        rgd::option_e       flip_horizontally;
        rgd::option_e       flip_vertically;
        rgd::image_layout_e image_layout;
        rgd::bit_depth_e    bit_depth;
        rgd::bool_t         premultiply_alpha;
        rgd::vector_4u      background_color;
        rgd::swizzle_e      swizzle;
    };

    auto check_image_format(std::span<const rgd::byte_t> image) -> rgd::image_format_e
    {
        if (mem::compare(image, bmp ::signature)) return rgd::image_format_e::bmp ;
        if (mem::compare(image, jpeg::signature)) return rgd::image_format_e::jpeg;
        if (mem::compare(image, png ::signature)) return rgd::image_format_e::png ;
        if (mem::compare(image, webp::signature)) return rgd::image_format_e::webp;

        throw std::invalid_argument{ "unsupported image format" };
    }
    auto decode           (std::optional<rgd::image_layout_e> image_layout, std::span<const rgd::byte_t> image_data) -> rgd::image
    {
        return png::decode(image_layout, image_data);
    }
}
