#ifndef COLLISION_HPP
#define COLLISION_HPP

#include "engine/entities.hpp"

void PlayerPlatformCollision(Player *player, Platform *platform,
                             CollisionState *collision_state);

void PlayerBlockCollision(Player *player, Block *block,
                          CollisionState *collision_state);

#endif  // COLLISION_HPP
