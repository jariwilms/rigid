export module rigid.types;

import std;

export namespace rgd
{
    using void_t        = void         ;
    using bool_t        = bool         ;
    using char_t        = char         ;
    using byte_t        = std::uint8_t ;
    using int8_t        = std::int8_t  ;
    using int16_t       = std::int16_t ;
    using int32_t       = std::int32_t ;
    using int64_t       = std::int64_t ;
    using uint8_t       = std::uint8_t ;
    using uint16_t      = std::uint16_t;
    using uint32_t      = std::uint32_t;
    using uint64_t      = std::uint64_t;
    using float32_t     = std::float_t ;
    using float64_t     = std::double_t;
    using size_t        = std::size_t  ;
    using cursor_t      = rgd::size_t  ;

    enum : rgd::bool_t
    {
        false_ = false, 
        true_  = true , 
    };

    template<typename T, rgd::uint32_t Components>
    struct vector;
    template<typename T>
    struct vector<T, 1u>                            { union { T x; T r; }; };
    template<typename T>
    struct vector<T, 2u> : public rgd::vector<T, 1u>{ union { T y; T g; }; };
    template<typename T>
    struct vector<T, 3u> : public rgd::vector<T, 2u>{ union { T z; T b; }; };
    template<typename T>
    struct vector<T, 4u> : public rgd::vector<T, 3u>{ union { T w; T a; }; };

    using vector_1i = rgd::vector<rgd::int32_t  , 1u>;
    using vector_2i = rgd::vector<rgd::int32_t  , 2u>;
    using vector_3i = rgd::vector<rgd::int32_t  , 3u>;
    using vector_4i = rgd::vector<rgd::int32_t  , 4u>;
    using vector_1u = rgd::vector<rgd::uint32_t , 1u>;
    using vector_2u = rgd::vector<rgd::uint32_t , 2u>;
    using vector_3u = rgd::vector<rgd::uint32_t , 3u>;
    using vector_4u = rgd::vector<rgd::uint32_t , 4u>;
    using vector_1f = rgd::vector<rgd::float32_t, 1u>;
    using vector_2f = rgd::vector<rgd::float32_t, 2u>;
    using vector_3f = rgd::vector<rgd::float32_t, 3u>;
    using vector_4f = rgd::vector<rgd::float32_t, 4u>;
}
