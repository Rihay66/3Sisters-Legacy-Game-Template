#include "../inc/animation.hpp"

// include C math library
#include <cmath>

// include glm math
#include <glm/common.hpp>

bool almost_equal(float a, float b, float epsilon){
    // approximation to being equal
    return glm::abs(a - b) <= epsilon;
}

bool animate_to_target_f32(float *value, float target, float delta_time, float rate, float epsilon){
    // change the value to move towards target according to delta time and rate
    *value += (target - *value) * (1.0f - powf(2.0, -rate * delta_time));
    // check if almost equal, ensures either no overshooting or smooth transition to the target
    if(almost_equal(*value, target, epsilon)){
        // set value to be exact to target
        *value = target;
        return true; // animation is done
    }

    return false; // animation is not finished
}

bool animate_to_target_vector2(glm::vec2 *value, glm::vec2 target, float delta_time, float rate, float epsilon){
    // do animation for x and y seperately
    return animate_to_target_f32(&value->x, target.x, delta_time, rate, epsilon) 
        && animate_to_target_f32(&value->y, target.y, delta_time, rate, epsilon);
}

bool animate_to_target_vector4(glm::vec4* value, glm::vec4 target, float delta_time, float rate, float epsilon){
    // do animation for x, y, and z seperately
    return animate_to_target_f32(&value->x, target.x, delta_time, rate, epsilon) 
        && animate_to_target_f32(&value->y, target.y, delta_time, rate, epsilon)
        && animate_to_target_f32(&value->z, target.z, delta_time, rate, epsilon)
        && animate_to_target_f32(&value->w, target.w, delta_time, rate, epsilon);
}
