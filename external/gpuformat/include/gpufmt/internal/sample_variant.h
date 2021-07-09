#pragma once

#include <gpufmt/config.h>

#include <glm/fwd.hpp>
#include <glm/vec4.hpp>

#include <variant>

namespace gpufmt {
    using SampleVariant = std::variant<glm::vec4, glm::i32vec4, glm::u32vec4, glm::dvec4
#ifndef GF_EXCLUDE_64BIT_FORMATS
        , glm::i64vec4, glm::u64vec4
#endif
    >;
}