export module rigid.decode.bmp;

import std;
import rigid.types;

export namespace rgd::bmp
{
    auto constexpr signature = std::array<rgd::byte_t, 2u>
    {
        0x42, 
        0x4D, 
    };
}
