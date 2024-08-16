#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "engine/entities.hpp"
#include "physics.hpp"

void Gravity(Player *player);
void JumpPhysics(Player *player, MotionState *motion_state);

#endif  // PHYSICS_HPP
