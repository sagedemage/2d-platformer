#ifndef COLLISION_H
#define COLLISION_H

#include "engine/entities.hpp"

void PlayerPlatformCollision(Player *player, Platform *platform,
                             CollisionState *collision_state);

void PlayerWallCollision(Player *player, Wall *wall,
                         CollisionState *collision_state);

#endif  // COLLISION_H
