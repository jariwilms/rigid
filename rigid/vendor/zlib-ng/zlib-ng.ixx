export module zlib_ng;

import std;
import <zlib-ng/zlib-ng.h>;

export namespace zlib
{
    auto inflate(std::span<const std::uint8_t> deflated_memory) -> std::vector<std::uint8_t>
    {
        auto       inflated_data = std::vector<std::uint8_t>{};
        auto       result        = std::int32_t{};
        auto       stream        = zng_stream{};
        auto const chunk_size    = std::size_t{ 16384u };
        auto const zng_init      = ::zng_inflateInit(&stream);
        if (zng_init != Z_OK) throw std::runtime_error{ "failed to initialize zlib-ng inflation: " + std::to_string(zng_init) };

        stream.next_in  = deflated_memory.data();
        stream.avail_in = static_cast<std::uint32_t>(deflated_memory.size_bytes());

        while (result != Z_STREAM_END)
        {
            auto const old_size = inflated_data.size();
            inflated_data.resize(old_size + chunk_size);

            stream.next_out  = inflated_data.data() + old_size;
            stream.avail_out = chunk_size;

            result = ::zng_inflate(&stream, Z_NO_FLUSH);
            if (result != Z_OK && result != Z_STREAM_END) 
            {
                ::zng_inflateEnd(&stream);
                throw std::runtime_error("failed to inflate data: " + std::to_string(result));
            }

            inflated_data.resize(old_size + chunk_size - stream.avail_out);
        }

        ::zng_inflateEnd(&stream);
        return inflated_data;
    }
}
