export module rigid.image;

import std;
import rigid.types;

export namespace rgd
{
    enum class image_format_e
    {
        bmp , 
        jpeg, 
        png , 
        webp, 
    };
    enum class image_layout_e
    {
        r    = 1u, 
        rg   = 2u, 
        rgb  = 3u, 
        rgba = 4u, 
    };
    class image
    {
    public:
        using layout_e = rgd::image_layout_e;

        layout_e                 layout;
        rgd::vector_2u           dimensions;
        std::vector<rgd::byte_t> data;
    };
}
