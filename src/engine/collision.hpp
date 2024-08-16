#ifndef COLLISION_HPP
#define COLLISION_HPP

#include "engine/entities.hpp"

void PlayerPlatformCollision(Player *player, Platform *platform,
                             CollisionState *collision_state);

void PlayerWallCollision(Player *player, Wall *wall,
                         CollisionState *collision_state);

#endif  // COLLISION_HPP
