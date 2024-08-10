#include "../entities/entities.hpp"

void playerPlatformCollision(Player *player, Platform *platform,
                             CollisionState *collisionState);

void playerWallCollision(Player *player, Wall walls[45],
                          CollisionState *collisionState);