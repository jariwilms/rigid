export module rigid.memory;

import std;
import rigid.types;

export namespace rgd::mem
{
    auto compare(std::span<const rgd::byte_t> left, std::span<const rgd::byte_t> right) -> rgd::bool_t
    {
        auto constexpr equal_result = rgd::int32_t{ 0 };
        return std::memcmp(left.data(), right.data(), std::min(left.size_bytes(), right.size_bytes())) == equal_result;
    }
    void copy   (std::span<const rgd::byte_t> from, std::span<rgd::byte_t> to)
    {
        std::memcpy(to.data(), from.data(), std::min(from.size_bytes(), to.size_bytes()));
    }
}
