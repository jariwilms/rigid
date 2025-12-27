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
    
    template<std::unsigned_integral T>
    auto constexpr test             (T value, rgd::size_t index) -> rgd::bool_t
    {
        return (value >> index) & 0x1;
    }
    template<std::unsigned_integral T>
    auto constexpr count            (T value) -> rgd::uint32_t
    {
        return std::popcount(value);
    }
    template<std::unsigned_integral T>
    auto constexpr is_power_of_2    (T value) -> rgd::bool_t
    {
        return std::has_single_bit(value);
    }
    
    template<typename T>
    auto constexpr swap_bytes       (T value) -> T
    {
        return std::byteswap(value);
    }
    template<typename T> requires (std::is_enum_v<T>)
    auto constexpr swap_bytes       (T value) -> T
    {
        return static_cast<T>(std::byteswap(std::to_underlying(value)));
    }
    template<bit::endian_e Endian, typename T>
    auto constexpr swap_bytes_native(T value) -> T
    {
        if   constexpr (Endian == bit::endian_e::native) return value;
        else                                             return bit::swap_bytes(value);
    }
    
    template<typename T>
    auto constexpr as_bytes         (T const& value) -> std::span<const rgd::byte_t>
    {
        auto const span      = std::span<const T>{ &value, 1u };
        auto const byte_span = std::span<const rgd::byte_t>{ reinterpret_cast<const rgd::byte_t*>(span.data()), span.size_bytes() };

        return byte_span;
    }
    
    auto constexpr pack             (std::span<const rgd::byte_t, 8u> value) -> rgd::byte_t
    {
        auto result = rgd::byte_t{};
        for (auto index = rgd::size_t{ 0u }; index < 8u; ++index)
        {
            result |= (bit::test(value[index], 0u) << index);
        }
        
        return result;
    }
    auto constexpr unpack           (rgd::byte_t value) -> std::array<rgd::byte_t, 8u>
    {
        return std::array<rgd::byte_t, 8u>
        {
            bit::test(value, 7u), 
            bit::test(value, 6u), 
            bit::test(value, 5u), 
            bit::test(value, 4u), 
            bit::test(value, 3u), 
            bit::test(value, 2u), 
            bit::test(value, 1u), 
            bit::test(value, 0u), 
        };
    }
}
