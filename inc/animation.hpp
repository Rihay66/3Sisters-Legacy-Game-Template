#pragma once

#ifndef ANIMATION_HPP
#define ANIMATION_HPP

// include glm 
#include <glm/glm.hpp>

//* Animation funcs

// animate a float over time
bool animate_to_target_f32(float* value, float target, float delta_time, float rate, float epsilon = 0.001f);

// animate two floating values, represented as a vector2, over time
bool animate_to_target_vector2(glm::vec2* value, glm::vec2 target, float delta_time, float rate, float epsilon = 0.001f);

// animate four float values, represented as a vector4, over time
bool animate_to_target_vector4(glm::vec4* value, glm::vec4 target, float delta_time, float rate, float epsilon = 0.001f);

#endif