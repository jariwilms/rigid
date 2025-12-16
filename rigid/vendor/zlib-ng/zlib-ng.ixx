export module zlib_ng;

import std;
import <zlib-ng/zlib-ng.h>;

export namespace zng
{
    using result_t = std::int32_t;
    using stream   = ::zng_stream;
    
    enum class result_e : zng::result_t
    {
        ok              = Z_OK           ,  
        stream_end      = Z_STREAM_END   ,  
        need_dictionary = Z_NEED_DICT    ,  
        error           = Z_ERRNO        , 
        stream_error    = Z_STREAM_ERROR , 
        data_error      = Z_DATA_ERROR   , 
        memory_error    = Z_MEM_ERROR    , 
        buffer_error    = Z_BUF_ERROR    , 
        version_error   = Z_VERSION_ERROR, 
    };
    enum class flush_e
    {
        no      = Z_NO_FLUSH     , 
        partial = Z_PARTIAL_FLUSH, 
        sync    = Z_SYNC_FLUSH   , 
        full    = Z_FULL_FLUSH   , 
        finish  = Z_FINISH       , 
        block   = Z_BLOCK        , 
        trees   = Z_TREES        , 
    };

    auto initialize_inflate(zng::stream& stream) -> zng::result_e
    {
        return static_cast<zng::result_e>(::zng_inflateInit(&stream));
    }
    auto inflate_stream    (zng::stream& stream, zng::flush_e flush) -> zng::result_e
    {
        return static_cast<zng::result_e>(::zng_inflate(&stream, std::to_underlying(flush)));
    }
    auto end_inflate       (zng::stream& stream) -> zng::result_e
    {
        return static_cast<zng::result_e>(::zng_inflateEnd(&stream));
    }
    
    auto inflate           (std::span<const std::uint8_t> deflated_memory) -> std::vector<std::uint8_t>
    {
        auto       stream        = zng::stream{};
        auto       result        = zng::result_e{};
        auto       inflated_data = std::vector<std::uint8_t>{};
        auto const chunk_size    = std::size_t{ 1u << 17u };
        auto const zng_init      = zng::initialize_inflate(stream);
        if (zng_init != zng::result_e::ok) throw std::runtime_error{ "failed to initialize zlib inflate" };

        stream.next_in           = deflated_memory.data();
        stream.avail_in          = static_cast<std::uint32_t>(deflated_memory.size_bytes());

        while (result != zng::result_e::stream_end)
        {
            auto const old_size = inflated_data.size();
            inflated_data.resize(old_size + chunk_size);

            stream.next_out     = inflated_data.data() + old_size;
            stream.avail_out    = chunk_size;
            result              = zng::inflate_stream(stream, zng::flush_e::no);
            if (result != zng::result_e::ok && result != zng::result_e::stream_end) 
            {
                zng::end_inflate(stream);
                throw std::runtime_error{ "failed to inflate data" };
            }

            inflated_data.resize(old_size + chunk_size - stream.avail_out);
        }

        zng::end_inflate(stream);
        return inflated_data;
    }
}
