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
std::enable_if_t<std::is_enum_v<EnumT>, std::underlying_type_t<EnumT>> ToUnderlying(EnumT e)
{
    return static_cast<std::underlying_type_t<EnumT>>(e);
}

template<class EnumT>
std::enable_if_t<std::is_enum_v<EnumT>, EnumT> IncrementEnum(EnumT e)
{
    return static_cast<EnumT>(ToUnderlying(e) + std::underlying_type_t<EnumT>(1));
}
} // namespace scrap