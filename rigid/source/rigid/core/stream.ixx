export module rigid.stream;

import std;
import rigid.types;
import rigid.bitwise;

export namespace rgd
{
    template<bit::endian_e Endian>
    class input_stream
    {
    public:
        input_stream(std::span<const rgd::byte_t> memory)
            : stream_{ memory } {}

        template<typename U>
        auto consume(auto...) -> U
        {
            auto value = U{};
            stream_.read(reinterpret_cast<rgd::byte_t*>(&value), sizeof(U));
            
            return bit::swap_bytes_native<Endian>(value);
        }
        template<>
        auto consume<std::string>(rgd::size_t length) -> std::string
        {
            auto value = std::string(length);
            stream_.read(value.data(), length);
            
            return value;
        }

        void forward (rgd::size_t offset)
        {
            stream_.seekg(offset, std::ios::cur);
        }
        void backward(rgd::size_t offset)
        {
            stream_.seekg(-offset, std::ios::cur);
        }

        auto position() -> rgd::size_t
        {
            return static_cast<rgd::size_t>(stream_.tellg());
        }

        operator rgd::bool_t()
        {
            return stream_.good();
        }

    private:
        std::basic_ispanstream<rgd::byte_t> stream_;
    };
}
