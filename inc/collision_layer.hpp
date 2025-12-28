#pragma once

#ifndef COLLISION_LAYER_HPP
#define COLLISION_LAYER_HPP

#include <cstdint>

/* Simple guide to layers and masks
    Collision layer: I am...
    Collision masks: I collide with...
*/

// define collision layers
typedef enum collision_layers{
    ENVIRONMENT_LAYER = 1,
    PLAYER_LAYER = 1 << 1,
    ITEM_LAYER = 1 << 2,
    ENEMY_LAYER = 1 << 3
} CollisionLayers;

// const definition of masks containing layers to collide with
const uint8_t playerMask = ENVIRONMENT_LAYER | ITEM_LAYER | ENEMY_LAYER;


#endif
