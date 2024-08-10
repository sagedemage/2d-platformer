#ifndef PHYSICS_H
#define PHYSICS_H

#include "entities.hpp"

void gravity(Player *player);
void jumpPhysics(Player *player, MotionState *jumpState);
void dropdownPhysics(Player *player, MotionState *jumpState);

#endif // PHYSICS_H