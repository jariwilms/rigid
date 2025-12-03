export module rigid.core;
export import rigid.bitwise;
export import rigid.stream;
export import rigid.types;

import std;

export namespace rgd
{
    enum class image_format_e
    {
        auto_,
        png, jpeg, bmp, webp,
    };
    enum class image_layout_e
    {
        r   , 
        rg  , 
        rgb , 
        rgba, 
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
