/*
    CREDITS: Dylan Falconer
    Link to his series on making a 2D game in OpenGL and C: 
        https://github.com/Falconerd/engine-from-scratch
*/

#pragma once

#ifndef SWEPT_PHYSICS_HPP
#define SWEPT_PHYSICS_HPP

#include <ecs/prebuilt_components/transform.hpp>

// Rigidbody alias
using Rigidbody = size_t;
// Static Rigidbody alias
using StaticRigidbody = size_t;

typedef struct hit Hit;
typedef struct dynamicBody DynamicBody;
typedef struct staticBody StaticBody;

// create callback templates

//? response callback of dynamic vs dynamic bodies 
typedef void (*On_Hit_Dynamic)(DynamicBody* self, DynamicBody* other, Hit hit);

//? response callback of dynamic vs static bodies
typedef void (*On_Hit_Static)(DynamicBody* self, StaticBody* other, Hit hit);

typedef struct aabb{
    glm::vec2 position;
    glm::vec2 halfSize;
} AABB;

struct dynamicBody{
    size_t id;
    AABB aabb;
    glm::vec2 velocity;
    glm::vec2 acceleration;
    uint8_t collision_layer;
    uint8_t collision_mask;
    bool isEnabled = true;
    bool setToDestroy = false;
    // ECS only
    unsigned int entity_id;
    // defined callbacks for this data struct
    On_Hit_Dynamic on_hit_dynamic;
    On_Hit_Static on_hit_static;
}; 

struct staticBody{
    size_t id;
    AABB aabb;
    uint8_t collision_layer;
    bool isEnabled = true;
    bool setToDestroy = false;
    // ECS only
    unsigned int entity_id;
};

struct hit{
    size_t otherID;
    float time;
    glm::vec2 position;
    glm::vec2 normal;
    bool is_hit = false;
};

// Initialize Physics world
void physicsInit(glm::vec2 gravity);
// Update Physics world
void physicsUpdate(double ts);

// Create a dynamic body, returns Rigidbody ID
Rigidbody physicsCreateDynamicBody(glm::vec2 position, glm::vec2 size, glm::vec2 velocity, uint8_t collision_layer, uint8_t collision_mask, On_Hit_Dynamic on_hit_dynamic, On_Hit_Static on_hit_static);
// Get Dynamic body by using Rigidbody ID 
DynamicBody* physicsGetDynamicBody(Rigidbody rb);

// Create a static body, returns Static Rigidbody ID
StaticRigidbody physicsCreateStaticBody(glm::vec2 position, glm::vec2 size, uint8_t collision_layer);
// Get Static body by using Static Rigidbody ID 
StaticBody* physicsGetStaticBody(StaticRigidbody rb);

// Destroy a dynamic body by using Rigidbody ID
void physicsDestroyDynamicBody(Rigidbody rb);
// Destroy a static body by using Static Rigidbody ID
void physicsDestroyStaticBody(StaticRigidbody rb);

// World reset, deletes all stored Dynamic and Static bodies
void physicsReset();

// checks for overlap between a point and a aabb
bool physicsPointIntersectAABB(glm::vec2 point, AABB aabb);
// checks for overlap between two aabb
bool physicsAABBIntersectAABB(AABB a, AABB b);
// return an aabb that is the sum of two other AABBs
AABB physicsMinkowskiDifference(AABB a, AABB b);
// returns a penetration vector according to a AABB difference
void physicsPenetrationVector(glm::vec2* r, AABB aabb);
// returns the min and max points of a AABB
void physicsAABBMinMax(glm::vec2* min, glm::vec2* max, AABB aabb);
// Casts a ray from one point to an AABB and returns where a hit
Hit physicsRayIntersectAABB(glm::vec2 position, glm::vec2 magnitude, AABB aabb);

#endif