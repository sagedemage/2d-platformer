#ifndef PHYSICS_H
#define PHYSICS_H

#include "engine/entities.hpp"
#include "physics.hpp"

void Gravity(Player *player);
void JumpPhysics(Player *player, MotionState *motion_state);

#endif  // PHYSICS_H
