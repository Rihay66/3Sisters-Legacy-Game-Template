#pragma once

#ifndef SWEPT_PHYSICS_INTERNAL_HPP
#define SWEPT_PHYSICS_INTERNAL_HPP

// include standard headers
#include <vector>
#include "../inc/swept_physics.hpp"

typedef struct{
    unsigned int iterations;
    float tickRate;
    float terminalVelocity;
    glm::vec2 gravity;
    std::vector<DynamicBody> dynamicBodyList;
    size_t createdDynamicBodies;
    std::vector<StaticBody> staticBodyList;
    size_t createdStaticBodies;
} PhysicsWorld;

#endif