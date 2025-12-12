export module zlib_ng;

import std;
import <zlib-ng/zlib-ng.h>;

export namespace zng
{
    enum
    {
        ok         = Z_OK        , 
        no_flush   = Z_NO_FLUSH  , 
        stream_end = Z_STREAM_END, 
    };

    using stream = ::zng_stream;

    auto initialize_inflate(zng::stream& stream) -> std::int32_t
    {
        return ::zng_inflateInit(&stream);
    }
    auto end_inflate       (zng::stream& stream) -> std::int32_t
    {
        return ::zng_inflateEnd(&stream);
    }
    auto inflate           (std::span<const std::uint8_t> deflated_memory) -> std::vector<std::uint8_t>
    {
        auto       stream        = zng::stream{};
        auto       result        = std::int32_t{};
        auto       inflated_data = std::vector<std::uint8_t>{};
        auto const chunk_size    = std::size_t{ 1u << 14u };
        auto const zng_init      = zng::initialize_inflate(stream);
        if (zng_init != zng::ok) throw std::runtime_error{ "failed to initialize zlib inflate, error_code: " + std::to_string(zng_init) };

        stream.next_in           = deflated_memory.data();
        stream.avail_in          = static_cast<std::uint32_t>(deflated_memory.size_bytes());

        while (result != zng::stream_end)
        {
            auto const old_size = inflated_data.size();
            inflated_data.resize(old_size + chunk_size);

            stream.next_out     = inflated_data.data() + old_size;
            stream.avail_out    = chunk_size;
            result              = ::zng_inflate(&stream, zng::no_flush);
            if (result != zng::ok && result != zng::stream_end) 
            {
                ::zng_inflateEnd(&stream);
                throw std::runtime_error("failed to inflate data: " + std::to_string(result));
            }

            inflated_data.resize(old_size + chunk_size - stream.avail_out);
        }

        zng::end_inflate(stream);
        return inflated_data;
    }
}
