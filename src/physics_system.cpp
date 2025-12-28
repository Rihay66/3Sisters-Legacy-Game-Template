/*
    CREDITS: Dylan Falconer
    Link to his series on making a 2D game in OpenGL and C: 
        https://github.com/Falconerd/engine-from-scratch
*/
#include "../inc/physics_system.hpp"

// include prebuilt components
#include "ecs/prebuilt_components/transform.hpp"

// include Game Physics
#include "../inc/swept_physics.hpp"

void PhysicsSystem::init(glm::vec2 gravity){
    // initialize physics by setting gravity
    physicsInit(gravity);
}

void PhysicsSystem::createRigidbody(Entity entity, uint8_t collision_mask, uint8_t collision_layer, glm::vec2 velocity, On_Hit_Dynamic dynamicResponse, On_Hit_Static staticResponse){
    // grab transform component
    auto& transform = ECS::GetComponent<Transform2D>(entity);
    // grab rigidbody component
    auto& rb = ECS::GetComponent<Rigidbody>(entity);
    // create dynamic body and set rigidbody
    rb = physicsCreateDynamicBody(transform.position, transform.size, velocity, collision_layer, collision_mask, dynamicResponse, staticResponse);
    // set entity to the dynamic body
    physicsGetDynamicBody(rb)->entity_id = entity;
}

//TODO: iff there's a need to access a static rigidbody then refactor physics code
void PhysicsSystem::createStaticRigidbody(Entity entity, uint8_t collision_layer){
    // grab transform component
    auto& transform = ECS::GetComponent<Transform2D>(entity);
    // create dynamic body and set rigidbody
    auto static_rb = physicsCreateStaticBody(transform.position, transform.size, collision_layer);
    // set entity to the static body
    physicsGetStaticBody(static_rb)->entity_id = entity;
}

void PhysicsSystem::update(double ts){
    // update physics
    physicsUpdate(ts);
    
    for(auto const& entity : entities){
        // grab components
        auto& transform = ECS::GetComponent<Transform2D>(entity);
        auto& rb = ECS::GetComponent<Rigidbody>(entity);
        
        // get dynamic body using rigidbody
        DynamicBody* body = physicsGetDynamicBody(rb);
        
        // update transform with returned body
        transform.position = body->aabb.position;
    }
}
