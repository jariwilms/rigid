export module rigid.core;
export import rigid.bitwise;
export import rigid.stream;
export import rigid.types;

import std;

export namespace rgd
{
    enum class image_format_e
    {
        auto_,
        png, jpeg, bmp, webp,
    };
    enum class image_layout_e
    {
        r   , 
        rg  , 
        rgb , 
        rgba, 
    };
    class image
    {
    public:
        using layout_e = rgd::image_layout_e;

        layout_e                 layout;
        rgd::vector_2u           dimensions;
        std::vector<rgd::byte_t> data;
    };

    template<typename T>
    class padded_mdspan_2d 
    {
    public:
        using mdspan_type = std::mdspan<T, std::dextents<size_t, 2>>;
        using index_type  = typename std::dextents<size_t, 2>::index_type;

        constexpr padded_mdspan_2d(T* pointer, index_type rows, index_type columns)
            : span_{ pointer, rows, columns }, rows_{ rows }, columns_{ columns } {}

        constexpr auto size  ()                            const noexcept -> index_type
        {
            return span_.size();
        }
        constexpr auto extent(mdspan_type::rank_type rank) const noexcept -> index_type
        {
            return span_.extent(rank);
        }

        constexpr auto operator[](index_type i, index_type j) const noexcept -> T
        {
            if   (i >= rows_ || j >= columns_ || i < 0u || j < 0u) return T{ 0u };
            else                                                   return span_[i, j];
        }

    private:
        mdspan_type span_;
        index_type  rows_;
        index_type  columns_;
    };
}
