export module rigid.core;
export import rigid.bitwise;
export import rigid.image;
export import rigid.memory;
export import rigid.stream;
export import rigid.types;

import std;

export namespace rgd
{
    template<auto V, auto Min, auto Max>
    concept in_inclusive_range = V >= Min && V <= Max;
    template<auto V, auto Min, auto Max>
    concept in_exclusive_range = V >  Min && V <  Max;
}
