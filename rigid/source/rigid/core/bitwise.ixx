export module rigid.bitwise;

import std;
import rigid.types;

export namespace rgd::bit
{
    enum class endian_e
    {
        little = std::endian::little, 
        big    = std::endian::big   , 
        native = std::endian::native, 
    };

    template<typename T>
    auto as_bytes(T const& value) -> std::span<const rgd::byte_t>
    {
        auto span      = std::span<const T>{ &value, 1u };
        auto byte_span = std::span<const rgd::byte_t>{ reinterpret_cast<const rgd::byte_t*>(span.data()), span.size_bytes() };

        return byte_span;
    }

    template<std::unsigned_integral T>
    auto test             (T value, rgd::size_t index) -> rgd::bool_t
    {
        return (value >> index) & 0x1;
    }
    template<std::unsigned_integral T>
    auto count            (T value) -> rgd::uint32_t
    {
        return std::popcount(value);
    }
    template<std::unsigned_integral T>
    auto is_power_of_2    (T value) -> rgd::bool_t
    {
        return std::has_single_bit(value);
    }
    
    template<typename T>
    auto swap_bytes       (T value) -> T
    {
        return std::byteswap(value);
    }
    template<typename T> requires (std::is_enum_v<T>)
    auto swap_bytes(T value) -> T
    {
        return static_cast<T>(std::byteswap(std::to_underlying(value)));
    }
    template<bit::endian_e E, typename T>
    auto swap_bytes_native(T value) -> T
    {
        if   constexpr (E == bit::endian_e::native) return value;
        else                                        return bit::swap_bytes(value);
    }
}
