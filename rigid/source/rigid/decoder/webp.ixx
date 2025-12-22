export module rigid.decode.webp;

import std;
import rigid.types;

export namespace rgd::webp
{
    auto constexpr signature = std::array<rgd::byte_t, 4u>
    {
        0x52, 
        0x49, 
        0x46, 
        0x46, 
    };
}
