#pragma once

#include <fmt/format.h>
// clang-format off
#include <glm/ext/quaternion_common.hpp>
#include <glm/detail/type_quat.hpp>
// clang-format on
#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_vec3.hpp>
#include <glm/detail/type_vec4.hpp>

template<class T, enum glm::qualifier Q>
struct fmt::formatter<glm::tvec2<T, Q>> : public fmt::formatter<std::string_view>
{
public:
    template<class FormatContext>
    auto format(const glm::tvec2<T, Q>& value, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "{}, {}", value.x, value.y);
    }
};

template<class T, enum glm::qualifier Q>
struct fmt::formatter<glm::tvec3<T, Q>> : public fmt::formatter<std::string_view>
{
public:
    template<class FormatContext>
    auto format(const glm::tvec3<T, Q>& value, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "{}, {}, {}", value.x, value.y, value.z);
    }
};

template<class T, enum glm::qualifier Q>
struct fmt::formatter<glm::tvec4<T, Q>> : public fmt::formatter<std::string_view>
{
public:
    template<class FormatContext>
    auto format(const glm::tvec4<T, Q>& value, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "{}, {}, {}, {}", value.x, value.y, value.z, value.w);
    }
};

template<class T, enum glm::qualifier Q>
struct fmt::formatter<glm::qua<T, Q>> : public fmt::formatter<std::string_view>
{
public:
    template<class FormatContext>
    auto format(const glm::qua<T, Q>& value, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "w:{}, x:{}, y:{}, z:{}", value.w, value.x, value.y, value.z);
    }
};