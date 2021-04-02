#pragma once

#define GLM_FORCE_CTOR_INIT
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <chrono>

namespace Horizon
{
    using Radians = float;
    using Degrees = float;

    using Vector2 = glm::vec2;
    using Vector3 = glm::vec3;
    using Vector4 = glm::vec4;

    using Vector2i = glm::ivec2;
    using Vector3i = glm::ivec3;
    using Vector4i = glm::ivec4;

    using Vector2u = glm::uvec2;
    using Vector3u = glm::uvec3;
    using Vector4u = glm::uvec4;

    using Vector2f = glm::fvec2;
    using Vector3f = glm::fvec3;
    using Vector4f = glm::fvec4;
                  
    using Vector2d = glm::dvec2;
    using Vector3d = glm::dvec3;
    using Vector4d = glm::dvec4;

    using Quaternion = glm::quat;

    using Matrix3 = glm::mat3;
    using Matrix4 = glm::mat4;

    const float PI = glm::pi<float>();

    const float EPSILON = 0.00001f;

    const Vector3 UNIT_X(1, 0, 0);
    const Vector3 UNIT_Y(0, 1, 0);
    const Vector3 UNIT_Z(0, 0, 1);
}