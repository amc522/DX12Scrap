#pragma once

#include <glm/vec3.hpp>
#include <array>

namespace scrap
{
template<class T>
class AABB
{
public:
    glm::tvec3<T> center{T(0.0), T(0.0), T(0.0)};
    glm::tvec3<T> extents{T(0.0), T(0.0), T(0.0)};

    AABB() = default;
    AABB(glm::tvec3<T> center_, glm::tvec3<T> extents_): center(center_), extents(extents_) {}

    constexpr glm::tvec3<T> getMin() const { return center - extents; }
    constexpr glm::tvec3<T> getMax() const { return center + extents; }
    constexpr glm::tvec3<T> getSize() const { return extents * T(2.0); }

    constexpr std::array<glm::tvec3<T>, 4> getFacePosX() const
    {
        const glm::tvec3<T> faceCenter = center + glm::tvec3<T>(extents.x, T(0.0), T(0.0));
        return {
            faceCenter + glm::tvec3<T>(T(0.0), extents.y, -extents.z),
            faceCenter + glm::tvec3<T>(T(0.0), -extents.y, -extents.z),
            faceCenter + glm::tvec3<T>(T(0.0), -extents.y, extents.z),
            faceCenter + glm::tvec3<T>(T(0.0), extents.y, extents.z),
        };
    }

    constexpr std::array<glm::tvec3<T>, 4> getFaceNegX() const
    {
        const glm::tvec3<T> faceCenter = center - glm::tvec3<T>(extents.x, T(0.0), T(0.0f));
        return {
            faceCenter + glm::tvec3<T>(T(0.0), extents.y, extents.z),
            faceCenter + glm::tvec3<T>(T(0.0), -extents.y, extents.z),
            faceCenter + glm::tvec3<T>(T(0.0), -extents.y, -extents.z),
            faceCenter + glm::tvec3<T>(T(0.0), extents.y, -extents.z),
        };
    }

    constexpr std::array<glm::tvec3<T>, 4> getFacePosY() const
    {
        const glm::tvec3<T> faceCenter = center + glm::tvec3<T>(T(0.0), extents.y, T(0.0));
        return {
            faceCenter + glm::tvec3<T>(-extents.x, T(0.0), extents.z),
            faceCenter + glm::tvec3<T>(-extents.x, T(0.0), -extents.z),
            faceCenter + glm::tvec3<T>(extents.x, T(0.0), -extents.z),
            faceCenter + glm::tvec3<T>(extents.x, T(0.0), extents.z),
        };
    }

    constexpr std::array<glm::tvec3<T>, 4> getFaceNegY() const
    {
        const glm::tvec3<T> faceCenter = center - glm::tvec3<T>(T(0.0), extents.y, T(0.0));
        return {
            faceCenter + glm::tvec3<T>(-extents.x, T(0.0), -extents.z),
            faceCenter + glm::tvec3<T>(-extents.x, T(0.0), extents.z),
            faceCenter + glm::tvec3<T>(extents.x, T(0.0), extents.z),
            faceCenter + glm::tvec3<T>(extents.x, T(0.0), -extents.z),
        };
    }

    constexpr std::array<glm::tvec3<T>, 4> getFacePosZ() const
    {
        const glm::tvec3<T> faceCenter = center + glm::tvec3<T>(T(0.0), T(0.0), extents.z);
        return {
            faceCenter + glm::tvec3<T>(extents.x, extents.y, T(0.0)),
            faceCenter + glm::tvec3<T>(extents.x, -extents.y, T(0.0)),
            faceCenter + glm::tvec3<T>(-extents.x, -extents.y, T(0.0)),
            faceCenter + glm::tvec3<T>(-extents.x, extents.y, T(0.0)),
        };
    }

    constexpr std::array<glm::tvec3<T>, 4> getFaceNegZ() const
    {
        const glm::tvec3<T> faceCenter = center - glm::tvec3<T>(T(0.0), T(0.0), extents.z);
        return {
            faceCenter + glm::tvec3<T>(-extents.x, extents.y, T(0.0)),
            faceCenter + glm::tvec3<T>(-extents.x, -extents.y, T(0.0)),
            faceCenter + glm::tvec3<T>(extents.x, -extents.y, T(0.0)),
            faceCenter + glm::tvec3<T>(extents.x, extents.y, T(0.0)),
        };
    }
};

using AABBf = AABB<float>;
using AABBd = AABB<double>;

} // namespace scrap