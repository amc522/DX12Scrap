#pragma once

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