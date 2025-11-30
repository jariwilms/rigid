export module rigid.image;

import std;
import rigid.core;

export namespace rgd
{
    enum class image_format_e
    {
        auto_,
        png, jpeg, bmp, webp,
    };
    class image
    {
    public:
        using format_e = rgd::image_format_e;

        image() = default;

        auto format    () const -> format_e
        {
            return format_;
        }
        auto dimensions() const -> rgd::vector_2u
        {
            return dimensions_;
        }
        auto data      () const -> const auto&
        {
            return data_;
        }

    private:
        format_e                 format_;
        rgd::vector_2u           dimensions_;
        std::vector<rgd::byte_t> data_;
    };
}
