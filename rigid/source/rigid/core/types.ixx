export module rigid.types;

import std;

export namespace rgd
{
    using void_t        = void;
    using bool_t        = bool;
    using char_t        = char;
    using byte_t        = std::uint8_t ;
    using int8_t        = std::int8_t  ;
    using int16_t       = std::int16_t ;
    using int32_t       = std::int32_t ;
    using int64_t       = std::int64_t ;
    using uint8_t       = std::uint8_t ;
    using uint16_t      = std::uint16_t;
    using uint32_t      = std::uint32_t;
    using uint64_t      = std::uint64_t;
    using float32_t     = std::float_t ;
    using float64_t     = std::double_t;

    template<typename... Ts>
    struct operator_overload : Ts...
    {
        using Ts::operator()...;
    };
}
