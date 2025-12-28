#pragma once

#ifndef PHYSICS_SYSTEM_HPP
#define PHYSICS_SYSTEM_HPP

// incldue ECS
#include <ecs/ecs.hpp>

// include glm
#include <glm/glm.hpp>

// include Game Physics
#include "../inc/swept_physics.hpp"

class PhysicsSystem : public System{
    public:
        // initialize game physics
        void init(glm::vec2 gravity = glm::vec2(0.0f, -9.81f));
        // create rigidbody
        void createRigidbody(Entity entity, uint8_t collision_mask, uint8_t collision_layer, glm::vec2 startingVelocity = glm::vec2(0.0f), On_Hit_Dynamic dynamicResponse = NULL, On_Hit_Static staticResponse = NULL);
        /* create static rigidbody.
            NOTE: Requires a static rigidbody to be set
        */
        void createStaticRigidbody(Entity entity, uint8_t collision_layer);
        /*  update entities with rigidbodies.
            NOTE: updates the transform of the entities
        */
        void update(double ts);
};

// define alias
using PhysicsSystem_Ptr = std::shared_ptr<PhysicsSystem>;

#endif