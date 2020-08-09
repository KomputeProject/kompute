#include "common.hpp"

#if defined(__ANDROID__)
android_app* global_android_app{ nullptr };
#endif

const vec3 Vectors::UNIT_X{ 1.0f, 0.0f, 0.0f };
const vec3 Vectors::UNIT_Y{ 0.0f, 1.0f, 0.0f };
const vec3 Vectors::UNIT_Z{ 0.0f, 0.0f, 1.0f };
const vec3 Vectors::UNIT_NEG_X{ -1.0f, 0.0f, 0.0f };
const vec3 Vectors::UNIT_NEG_Y{ 0.0f, -1.0f, 0.0f };
const vec3 Vectors::UNIT_NEG_Z{ 0.0f, 0.0f, -1.0f };
const vec3 Vectors::UNIT_XY{ glm::normalize(UNIT_X + UNIT_Y) };
const vec3 Vectors::UNIT_XZ{ glm::normalize(UNIT_X + UNIT_Z) };
const vec3 Vectors::UNIT_YZ{ glm::normalize(UNIT_Y + UNIT_Z) };
const vec3 Vectors::UNIT_XYZ{ glm::normalize(UNIT_X + UNIT_Y + UNIT_Z) };
const vec3 Vectors::MAX{ FLT_MAX };
const vec3 Vectors::MIN{ -FLT_MAX };
const vec3 Vectors::ZERO{ 0.0f };
const vec3 Vectors::ONE{ 1.0f };
const vec3 Vectors::TWO{ 2.0f };
const vec3 Vectors::HALF{ 0.5f };
const vec3& Vectors::RIGHT = Vectors::UNIT_X;
const vec3& Vectors::UP = Vectors::UNIT_Y;
const vec3& Vectors::FRONT = Vectors::UNIT_NEG_Z;

const quat Rotations::IDENTITY{ 1.0f, 0.0f, 0.0f, 0.0f };
const quat Rotations::Y_180{ 0.0f, 0.0f, 1.0f, 0.0f };
