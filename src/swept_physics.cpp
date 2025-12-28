#include "../inc/swept_physics.hpp"
#include "../inc/swept_physics_internal.hpp"

#include <cmath>
#include <cstddef>

static PhysicsWorld world;

void physicsInit(glm::vec2 gravity){
    world.dynamicBodyList.reserve(sizeof(DynamicBody));
    world.staticBodyList.reserve(sizeof(StaticBody));
    
    //* User defined world settings
    world.gravity = gravity;
    world.terminalVelocity = -10000.0f;
    
    world.iterations = 6;
    world.tickRate = 1.0f / (float)world.iterations;
    
    world.createdDynamicBodies = 0;
    world.createdStaticBodies = 0;
}

static DynamicBody* physicsGetDynamicBodyIndex(unsigned int index){
    return &world.dynamicBodyList.at(index);
}

static StaticBody* physicsGetStaticBodyIndex(unsigned int index){
    return &world.staticBodyList.at(index);
}

static void physicsSweepBodiesResult(Hit* result, DynamicBody* body, size_t other_id, glm::vec2 velocity){
    // grab other dynamic body
    DynamicBody* other = physicsGetDynamicBodyIndex(other_id);
    
    // check if a's mask matches with b's layer
    if((body->collision_mask & other->collision_layer) == 0){
        return; // layers don't match, no need to check for collision
    }

    AABB sumAABB = other->aabb;
    sumAABB.halfSize = sumAABB.halfSize + body->aabb.halfSize;
    
    Hit hit = physicsRayIntersectAABB(body->aabb.position, velocity, sumAABB);
    
    if(hit.is_hit){
        // call dynamic callback
        if(body->on_hit_dynamic != NULL && (body->collision_mask & other->collision_layer) == 0){
            body->on_hit_dynamic(body, other, hit);
        }
        
        if(hit.time < result->time){
            *result = hit;
        }else if(hit.time == result->time){
            // solve for highest velocity axis first
            if(fabsf(velocity.x) > fabsf(velocity.y) && hit.normal.x != 0){
                *result = hit;
            }else if (fabsf(velocity.y) > fabsf(velocity.x) && hit.normal.y != 0) {
                *result = hit;
            }
        }
        // update result's ID to the aabb that collided
        result->otherID = other_id;
    }
}

static void physicsSweepStaticBodiesResult(Hit* result, DynamicBody* body, size_t other_id, glm::vec2 velocity){
    // grab other static body
    StaticBody* other = physicsGetStaticBodyIndex(other_id);
    
    // check if a's mask matches with b's layer
    if((body->collision_mask & other->collision_layer) == 0){
        return; // layers don't match, no need to check for collision
    }

    AABB sumAABB = other->aabb;
    sumAABB.halfSize = sumAABB.halfSize + body->aabb.halfSize;
    
    Hit hit = physicsRayIntersectAABB(body->aabb.position, velocity, sumAABB);
    
    if(hit.is_hit){
        if(hit.time < result->time){
            *result = hit;
        }else if(hit.time == result->time){
            // solve for highest velocity axis first
            if(fabsf(velocity.x) > fabsf(velocity.y) && hit.normal.x != 0){
                *result = hit;
            }else if (fabsf(velocity.y) > fabsf(velocity.x) && hit.normal.y != 0) {
                *result = hit;
            }
        }
        // update result's ID to the aabb that collided
        result->otherID = other_id;
    }
}


static Hit physicsSweepStaticBodies(DynamicBody* body, glm::vec2 velocity){
    Hit result = {.time = 0xBEEF};
    
    for(unsigned int i = 0; i < world.staticBodyList.size(); i++){
        StaticBody* other = physicsGetStaticBodyIndex(i);
        
        // if static object is not enabled
        if(!other->isEnabled){
            continue; // skip iteration
        }
        
        physicsSweepStaticBodiesResult(&result, body, i, velocity);
    }
    
    return result;
}

static Hit physicsSweepDynamicBodies(DynamicBody* body, glm::vec2 velocity){
    Hit result = {.time = 0xBEEF};
    
    for(unsigned int i = 0; i < world.dynamicBodyList.size(); i++){
        DynamicBody* other = physicsGetDynamicBodyIndex(i);
        
        // avoid checking if the passed body is the same as the world dynamic body
        if (body == other) {
			continue;
		}
        
        physicsSweepBodiesResult(&result, body, i, velocity);
    }
    
    return result;
}


static void physicsSweepResponse(DynamicBody* body, glm::vec2 scaledVelocity){
    Hit hit_static = physicsSweepStaticBodies(body, scaledVelocity);
    Hit hit_dynamic = physicsSweepDynamicBodies(body, scaledVelocity);
    
    // apply dynamic response
    if(hit_dynamic.is_hit){
        if(body->on_hit_dynamic != NULL){
            body->on_hit_dynamic(body, physicsGetDynamicBodyIndex(hit_dynamic.otherID), hit_dynamic);
        }
    }
    
    // apply static response
    if(hit_static.is_hit){
        body->aabb.position = hit_static.position;
        
        if(hit_static.normal.x != 0){
            body->aabb.position.y += scaledVelocity.y;
            body->velocity.x = 0;
        }else if(hit_static.normal.y != 0){
            body->aabb.position.x += scaledVelocity.x;
            body->velocity.y = 0;
        }
        
        // check for if the dynamic has a set static response
        if(body->on_hit_static != NULL){
            body->on_hit_static(body, physicsGetStaticBodyIndex(hit_static.otherID), hit_static);
        }
    }else{
        body->aabb.position += scaledVelocity; 
    }
}

static void physicsStationaryResponse(DynamicBody* body){
    for(unsigned int i = 0; i < world.staticBodyList.size(); i++){
        StaticBody* staticBody = physicsGetStaticBodyIndex(i);
        
        // if static object is not enabled
        if(!staticBody->isEnabled){
            continue; // skip iteration
        }
        // if static object is set to be destroyed
        if(staticBody->setToDestroy){
            physicsDestroyStaticBody(staticBody->id);
            break; // get out of loop
        }
        
        // check if the masks and layers match
        //* NOTE: Essentially dynamic bodies fall through static bodies if their collision layers don't match
        if((body->collision_mask & staticBody->collision_layer) == 0){
            continue; // skip if they don't match
        }
        
        //? Apply collision between dynamic and static bodies 
        
        AABB aabb = physicsMinkowskiDifference(staticBody->aabb,  body->aabb);
        
        glm::vec2 min, max;
        physicsAABBMinMax(&min, &max, aabb);
        
        if(min.x <= 0 && max.x >= 0 && min.y <= 0 && max.y >= 0){
            glm::vec2 penetration;
            physicsPenetrationVector(&penetration, aabb);
            body->aabb.position += penetration;
        }
    }
}

void physicsUpdate(double ts){
    // temporary ptr
    DynamicBody* body;
    
    // loop through all dynamic bodies
    for(unsigned int i = 0; i < world.dynamicBodyList.size(); i++){
        body = &world.dynamicBodyList.at(i);
        
        // skip dynamic bodies that aren't enabled
        if(body->isEnabled == false){
            continue; // skip to the next iteration
        }
        // destroy bodies that are set to be destroyed
        if(body->setToDestroy == true){
            physicsDestroyDynamicBody(body->id);
            break; // get out of loop
        }
        
        body->velocity += world.gravity;
        if((world.terminalVelocity > body->velocity.x || world.terminalVelocity > body->velocity.y)){
            body->velocity = glm::vec2(world.terminalVelocity);
        }
        
        // update the velocity of the body
        body->velocity.x += body->acceleration.x; 
        body->velocity.y += body->acceleration.y;

        glm::vec2 scaledVelocity = body->velocity * (((float)ts * world.tickRate));
        
        //TODO: Implement Board and Narrow Phase Sweep to allow for optimization
        
        // Dynamic vs Stationary Bodies 
        for(unsigned int i = 0; i < world.iterations; i++){
            physicsSweepResponse(body, scaledVelocity);
            physicsStationaryResponse(body);
        }
    }
}

Rigidbody physicsCreateDynamicBody(glm::vec2 position, glm::vec2 size, glm::vec2 velocity, uint8_t collision_layer, uint8_t collision_mask, On_Hit_Dynamic on_hit_dynamic, On_Hit_Static on_hit_static){
    // create the body
    DynamicBody body = {
        .id = world.createdDynamicBodies,
        .aabb = {
            .position = position,
            .halfSize = {size.x * 0.5f, size.y * 0.5f}
        },
        .velocity = velocity,
        .collision_layer = collision_layer,
        .collision_mask = collision_mask,
        .on_hit_dynamic = on_hit_dynamic,
        .on_hit_static = on_hit_static
    };
    
    // add body to world
    world.dynamicBodyList.push_back(body);
    
    // increment created bodies
    world.createdDynamicBodies++;
    
    return body.id;
}

StaticRigidbody physicsCreateStaticBody(glm::vec2 position, glm::vec2 size, uint8_t collision_layer){
    // create the body
    StaticBody body = { 
        .id = world.createdStaticBodies,
        .aabb = {
            .position = position,
            .halfSize = {size.x * 0.5f, size.y * 0.5f}
        },
        .collision_layer = collision_layer
    };
    
    // add body to world
    world.staticBodyList.push_back(body);
    
    // increment created bodies
    world.createdStaticBodies++;
    
    return body.id;
}

bool physicsPointIntersectAABB(glm::vec2 point, AABB aabb){
    // check for overlap
    glm::vec2 min, max;
    physicsAABBMinMax(&min, &max, aabb);
    return point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y;
}

bool physicsAABBIntersectAABB(AABB a, AABB b){
    // check for overlap
    glm::vec2 min, max;
    
    physicsAABBMinMax(&min, &max, physicsMinkowskiDifference(a, b));
    
    return (min.x <= 0 && max.x >= 0 && min.y <= 0 && max.y >= 0);
}

AABB physicsMinkowskiDifference(AABB a, AABB b){
    AABB result;
    
    result.position = glm::vec2(a.position.x - b.position.x, a.position.y - b.position.y);
    result.halfSize = glm::vec2(a.halfSize.x + b.halfSize.x, a.halfSize.y + b.halfSize.y);
    
    return result;
}

void physicsPenetrationVector(glm::vec2* r, AABB aabb){
    glm::vec2 min, max;
    
    physicsAABBMinMax(&min, &max, aabb);
    
    float minDist = fabsf(min.x);
    r->x = min.x;
    r->y = 0.0f;
    
    if(fabsf(max.x) < minDist){
        minDist = fabsf(max.x);
        r->x = max.x;
    }
    
    if(fabsf(min.y) < minDist){
        minDist = fabsf(min.y);
        r->x = 0.0f;
        r->y = min.y;
    }
    
    if(fabsf(max.y) < minDist){
        r->x = 0.0f;
        r->y = max.y;
    }
}

void physicsAABBMinMax(glm::vec2* min, glm::vec2* max, AABB aabb){
    *min = aabb.position - (aabb.halfSize);
    *max = aabb.position + (aabb.halfSize);
}

Hit physicsRayIntersectAABB(glm::vec2 position, glm::vec2 magnitude, AABB aabb){
    Hit hit = {.time = 0.0f, .position = position, .is_hit = false};

    glm::vec2 min, max;
    physicsAABBMinMax(&min, &max, aabb);
    
    float t1 = ((min[0] - position[0]) / magnitude[0]);
    float t2 = ((max[0] - position[0]) / magnitude[0]);
    
    float last_entry = fminf(t1, t2);
    float first_exit = fmaxf(t1, t2);

    for(int i = 0; i < 2; i++){
        if(magnitude[i] != 0){
            t1 = ((min[i] - position[i]) / magnitude[i]);
            t2 = ((max[i] - position[i]) / magnitude[i]);

            last_entry = fmaxf(last_entry, fminf(t1, t2));
            first_exit = fminf(first_exit, fmaxf(t1, t2));
        }else if(position[i] <= min[i] || position[i] >= max[i]){
            return hit;
        }
    }

    if(first_exit > last_entry && first_exit > 0 && last_entry < 1){
        hit.position.x = position.x + magnitude.x * last_entry;
        hit.position.y = position.y + magnitude.y * last_entry;

        hit.is_hit = true;
        hit.time = last_entry;
        
        float dx = hit.position.x - aabb.position.x;
        float dy = hit.position.y - aabb.position.y;
        float px = aabb.halfSize.x - fabsf(dx);
        float py = aabb.halfSize.y - fabsf(dy);
        
        if(px < py){
            hit.normal.x = (dx > 0) - (dx < 0);
        }else{
            hit.normal.y = (dy > 0) - (dy < 0); 
        }
    }

    return hit;
}

DynamicBody* physicsGetDynamicBody(Rigidbody rb){
    // linear search of rigidbody 
    for(unsigned int i = 0; i < world.dynamicBodyList.size(); i++){
        DynamicBody* body = physicsGetDynamicBodyIndex(i);
        // check if id's match
        if(body->id == rb){
            return body; // return the appropriate body
        }
    }
    
    //? display error
    
    return NULL; // return nothing
}

StaticBody* physicsGetStaticBody(StaticRigidbody rb){
    // linear search of rigidbody 
    for(unsigned int i = 0; i < world.staticBodyList.size(); i++){
        StaticBody* body = physicsGetStaticBodyIndex(i);
        // check if id's match
        if(body->id == rb){
            return body; // return the appropriate body
        }
    }
    
    //? display error
    
    return NULL; // return nothing
}

void physicsDestroyDynamicBody(Rigidbody rb){
    // destroy dynamic body with similar rb ID
    
    // linear search of rigidbody
    for(unsigned int i = 0; i < world.dynamicBodyList.size(); i++){
        DynamicBody* body = physicsGetDynamicBodyIndex(i);
        // check if id's match
        if(body->id == rb){
            // erase body from list
            world.dynamicBodyList.erase(world.dynamicBodyList.begin() + i);
            
            return; // stop function
        }
    }
    
    //? display error
    
}

void physicsDestroyStaticBody(StaticRigidbody rb){
    // destroy static body with similar rb ID
    
    // linear search of rigidbody
    for(unsigned int i = 0; i < world.staticBodyList.size(); i++){
        StaticBody* body = physicsGetStaticBodyIndex(i);
        // check if id's match
        if(body->id == rb){
            // erase body from list
            world.staticBodyList.erase(world.staticBodyList.begin() + i);
            
            return; // stop function
        }
    }
    
    //? display error
    
}

void physicsReset(){
    // delete all bodies
    world.dynamicBodyList.clear();
    world.staticBodyList.clear();
}