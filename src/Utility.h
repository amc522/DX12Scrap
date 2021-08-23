#pragma once

#include <type_traits>

#define DEFINE_ENUM_BITWISE_OPERATORS(EnumClassName)                                                  \
    constexpr inline EnumClassName operator|(EnumClassName left, EnumClassName right)                 \
    {                                                                                                 \
        return static_cast<EnumClassName>(static_cast<std::underlying_type_t<EnumClassName>>(left) |  \
                                          static_cast<std::underlying_type_t<EnumClassName>>(right)); \
    }                                                                                                 \
    constexpr inline EnumClassName operator&(EnumClassName left, EnumClassName right)                 \
    {                                                                                                 \
        return static_cast<EnumClassName>(static_cast<std::underlying_type_t<EnumClassName>>(left) &  \
                                          static_cast<std::underlying_type_t<EnumClassName>>(right)); \
    }                                                                                                 \
    constexpr inline EnumClassName operator^(EnumClassName left, EnumClassName right)                 \
    {                                                                                                 \
        return static_cast<EnumClassName>(static_cast<std::underlying_type_t<EnumClassName>>(left) ^  \
                                          static_cast<std::underlying_type_t<EnumClassName>>(right)); \
    }                                                                                                 \
    constexpr inline EnumClassName operator~(EnumClassName val)                                       \
    {                                                                                                 \
        return static_cast<EnumClassName>(~static_cast<std::underlying_type_t<EnumClassName>>(val));  \
    }                                                                                                 \
    constexpr inline EnumClassName& operator|=(EnumClassName& left, EnumClassName right)              \
    {                                                                                                 \
        left = static_cast<EnumClassName>(static_cast<std::underlying_type_t<EnumClassName>>(left) |  \
                                          static_cast<std::underlying_type_t<EnumClassName>>(right)); \
        return left;                                                                                  \
    }                                                                                                 \
    constexpr inline EnumClassName& operator&=(EnumClassName& left, EnumClassName right)              \
    {                                                                                                 \
        left = static_cast<EnumClassName>(static_cast<std::underlying_type_t<EnumClassName>>(left) &  \
                                          static_cast<std::underlying_type_t<EnumClassName>>(right)); \
        return left;                                                                                  \
    }                                                                                                 \
    constexpr inline EnumClassName& operator^=(EnumClassName& left, EnumClassName right)              \
    {                                                                                                 \
        left = static_cast<EnumClassName>(static_cast<std::underlying_type_t<EnumClassName>>(left) ^  \
                                          static_cast<std::underlying_type_t<EnumClassName>>(right)); \
        return left;                                                                                  \
    }

namespace scrap
{
template<class EnumT>
constexpr std::enable_if_t<std::is_enum_v<EnumT>, std::underlying_type_t<EnumT>> ToUnderlying(EnumT e)
{
    return static_cast<std::underlying_type_t<EnumT>>(e);
}

template<class EnumT>
constexpr std::enable_if_t<std::is_enum_v<EnumT>, EnumT> IncrementEnum(EnumT e)
{
    return static_cast<EnumT>(ToUnderlying(e) + std::underlying_type_t<EnumT>(1));
}

template<class... Ts>
struct Overloaded : Ts...
{
    using Ts::operator()...;
};
template<class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

template<typename T, size_t N>
constexpr size_t GetArraySize(const T(&)[N])
{
    return N;
}

template<typename T>
constexpr T AlignInteger(T value, T alignment)
{
    static_assert(std::is_integral_v<T>);
    return (value + (alignment - T(1))) & (~(alignment - T(1)));
}
} // namespace scrap