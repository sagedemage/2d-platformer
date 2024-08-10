#ifndef COLLISION_H
#define COLLISION_H

#include "engine/entities.hpp"

void playerPlatformCollision(Player *player, Platform *platform,
                             CollisionState *collisionState);

void playerWallCollision(Player *player, Wall walls[45],
                          CollisionState *collisionState);

#endif // COLLISION_H