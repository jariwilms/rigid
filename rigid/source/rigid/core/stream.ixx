export module rigid.stream;

import std;
import rigid.types;
import rigid.bitwise;

export namespace rgd
{
    template<typename T>
    auto lift(T value, auto function) -> decltype(std::invoke(function, value))
    {
        return std::invoke(function, value);
    }
    template<typename T> requires (std::is_enum_v<T>)
    auto lift(T value, auto function) -> decltype(std::invoke(function, std::to_underlying(value)))
    {
        return std::invoke(function, std::to_underlying(value));
    }



    template<typename T> requires (std::is_trivially_copyable_v<T>)
    auto consume(std::span<const rgd::byte_t> memory, rgd::cursor_t& cursor) -> T
    {
        if (cursor + sizeof(T) > memory.size_bytes()) throw std::out_of_range{ "invalid memory access" };

        auto value = T{};
        std::memcpy(&value, memory.data() + cursor, sizeof(T));
        cursor += sizeof(T);

        return value;
    }
    template<typename T> requires (std::is_trivially_copyable_v<T>)
    void produce(std::span<const rgd::byte_t> memory, rgd::cursor_t& cursor, T value)
    {
        if (cursor + sizeof(T) > memory.size_bytes()) throw std::out_of_range{ "invalid memory access" };

        std::memcpy(memory.data() + cursor, &value, sizeof(T));
        cursor += sizeof(T);
    }

    template<typename T>
    auto consume_b(std::span<const rgd::byte_t> memory, rgd::cursor_t& cursor) -> T
    {
        auto const value = rgd::consume<T>(memory, cursor);
        return bit::swap_bytes_native<bit::endian_e::big>(value);
    }
}
