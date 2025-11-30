export module rigid;
export import rigid.decode.png;
export import rigid.decode.jpeg;
export import rigid.decode.webp;
export import rigid.decode.bmp;

import std;
import rigid.core;
import rigid.image;

export namespace rgd
{
    auto image_type_by_signature(std::span<const rgd::byte_t> image) -> rgd::image_format_e
    {
        constexpr auto maximum_signature_size = std::size_t{ 12u };
        if (image.size_bytes() >= maximum_signature_size)
        {
            if (std::memcmp(image.data(), &png ::signature, sizeof(png ::signature)) == 0u) return rgd::image_format_e::png ;
            if (std::memcmp(image.data(), &jpeg::signature, sizeof(jpeg::signature)) == 0u) return rgd::image_format_e::jpeg;
            if (std::memcmp(image.data(), &bmp ::signature, sizeof(bmp ::signature)) == 0u) return rgd::image_format_e::bmp ;
            if (std::memcmp(image.data(), &webp::signature, sizeof(webp::signature)) == 0u) return rgd::image_format_e::webp;
        }

        throw std::invalid_argument{ "unsupported image format" };
    }
    template<rgd::image_format_e Format = rgd::image_format_e::auto_>
    auto decode                 (std::span<const rgd::byte_t> image)
    {
        return png::decode(image);
    }
}
