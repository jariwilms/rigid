export module rigid.decode.jpeg;

import std;
import rigid.types;

export namespace rgd::jpeg
{
    auto constexpr signature = std::array<rgd::byte_t, 3u>
    {
        0xFF,
        0xD8,
        0xFF,
    };
}
