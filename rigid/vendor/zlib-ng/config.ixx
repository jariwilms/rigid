export module zlib_ng.config;

import std;
import <zlib-ng/zlib-ng.h>;

export namespace zng::config
{
    auto constexpr chunk_size = std::size_t{ 1u << 20u };
}
