export module rigid.core;
export import rigid.types;

import std;
import <zlib-ng/zlib-ng.h>;

export namespace rgd
{
    auto inflate(const std::vector<rgd::byte_t>& compressedData) -> std::vector<rgd::byte_t>
    {
        auto stream = zng_stream{};
        auto result = std::int32_t{};
        
        result = zng_inflateInit(&stream);
        if (result != Z_OK) throw std::runtime_error{ "failed to initialize zlib-ng inflation: " + std::to_string(result) };

        stream.next_in  = compressedData.data();
        stream.avail_in = static_cast<std::uint32_t>(compressedData.size());

              auto decompressed_data = std::vector<rgd::byte_t>{};
        const auto chunk_size        = size_t{ 16384u };

        do 
        {
            const auto old_size = std::size_t{ decompressed_data.size() };
            decompressed_data.resize(old_size + chunk_size);

            stream.next_out  = decompressed_data.data() + old_size;
            stream.avail_out = chunk_size;

            result = zng_inflate(&stream, Z_NO_FLUSH);
            if (result != Z_OK && result != Z_STREAM_END) 
            {
                zng_inflateEnd(&stream);
                throw std::runtime_error("Failed to inflate data: " + std::to_string(result));
            }

            decompressed_data.resize(old_size + chunk_size - stream.avail_out);

        } while (result != Z_STREAM_END);

        zng_inflateEnd(&stream);
        
        return decompressed_data;
    }
    template<typename T>
    auto interpret_as(      rgd::byte_t* pointer, const std::size_t offset = 0u) -> T&
    {
        return *std::bit_cast<T*>(pointer + offset);
    }
    template<typename T>
    auto view_as     (const rgd::byte_t* pointer, const std::size_t offset = 0u) -> const T&
    {
        return *std::bit_cast<const T*>(pointer + offset);
    }
    template<typename T>
    auto byte_swap   (const T& value) -> T
    {
        if   constexpr (std::is_enum_v<T>) return static_cast<T>(std::byteswap(std::to_underlying(value)));
        else                               return                std::byteswap(                   value  );
    }

    template<typename T, std::endian E>
    class endian_view
    {
    public:
        auto to_native() const -> T
        {
            if   constexpr (E == std::endian::native) return                value_;
            else                                      return rgd::byte_swap(value_);
        }
        operator T() const
        {
            return to_native();
        }

    private:
        T value_;
    };
    template<typename T>
    using little_endian_view = endian_view<T, std::endian::little>;
    template<typename T>
    using big_endian_view    = endian_view<T, std::endian::big   >;
}
