#include "../entities/entities.hpp"

void playerWallCollision(Player *player, Wall *wall,
                         CollisionState *collisionState) {
    int offset = 5;
    /* X Axis Collision */
    if (player->dstrect.y > wall->dstrect.y + offset &&
        player->dstrect.y < wall->dstrect.y + 24 - offset) {
        if (player->dstrect.x + 24 > wall->dstrect.x &&
            player->dstrect.x + 24 < wall->dstrect.x + 24) {
            // left collision
            player->dstrect.x -= player->speed;
        } else if (player->dstrect.x < wall->dstrect.x + 24 &&
                   player->dstrect.x > wall->dstrect.x) {
            // right collision
            player->dstrect.x += player->speed;
        }
    }

    else if (player->dstrect.y + 24 > wall->dstrect.y + offset &&
             player->dstrect.y + 24 < wall->dstrect.y + 24 - offset) {
        if (player->dstrect.x + 24 > wall->dstrect.x &&
            player->dstrect.x + 24 < wall->dstrect.x + 24) {
            // left collision
            player->dstrect.x -= player->speed;
        } else if (player->dstrect.x < wall->dstrect.x + 24 &&
                   player->dstrect.x > wall->dstrect.x) {
            // right collision
            player->dstrect.x += player->speed;
        }
    }

    else if (player->dstrect.y + 24 / 2 > wall->dstrect.y &&
             player->dstrect.y + 24 / 2 < wall->dstrect.y + 24) {
        if (player->dstrect.x + 24 > wall->dstrect.x &&
            player->dstrect.x + 24 < wall->dstrect.x + 24) {
            // left collision
            player->dstrect.x -= player->speed;
        } else if (player->dstrect.x < wall->dstrect.x + 24 &&
                   player->dstrect.x > wall->dstrect.x) {
            // right collision
            player->dstrect.x += player->speed;
        }
    }

    /* Y Axis Collision */
    if (player->dstrect.x > wall->dstrect.x &&
        player->dstrect.x < wall->dstrect.x + 24) {
        if (player->dstrect.y + 24 > wall->dstrect.y &&
            player->dstrect.y + 24 < wall->dstrect.y + 24) {
            // top collision
            player->dstrect.y -= player->accel;
            collisionState->on_the_floor = true;
        } else if (player->dstrect.y < wall->dstrect.y + 24 &&
                   player->dstrect.y > wall->dstrect.y) {
            // bottom collision
            player->dstrect.y += player->accel;
        }
    }

    else if (player->dstrect.x + 24 > wall->dstrect.x &&
             player->dstrect.x + 24 < wall->dstrect.x + 24) {
        if (player->dstrect.y + 24 > wall->dstrect.y &&
            player->dstrect.y + 24 < wall->dstrect.y + 24) {
            // top collision
            player->dstrect.y -= player->accel;
            collisionState->on_the_floor = true;
        } else if (player->dstrect.y < wall->dstrect.y + 24 &&
                   player->dstrect.y > wall->dstrect.y) {
            // bottom collision
            player->dstrect.y += player->accel;
        }
    }

    else if (player->dstrect.x + 24 / 2 > wall->dstrect.x &&
             player->dstrect.x + 24 / 2 < wall->dstrect.x + 24) {
        if (player->dstrect.y + 24 > wall->dstrect.y &&
            player->dstrect.y + 24 < wall->dstrect.y + 24) {
            // top collision
            player->dstrect.y -= player->accel;
            collisionState->on_the_floor = true;
        } else if (player->dstrect.y < wall->dstrect.y + 24 &&
                   player->dstrect.y > wall->dstrect.y) {
            // bottom collision
            player->dstrect.y += player->accel;
        }
    }
}

void playerPlatformCollision(Player *player, Platform *platform,
                             CollisionState *collisionState) {
    /* Y Axis Collision */
    if (player->dstrect.x > platform->dstrect.x &&
        player->dstrect.x < platform->dstrect.x + 24) {
        if (player->dstrect.y + 24 > platform->dstrect.y &&
            player->dstrect.y + 24 < platform->dstrect.y + 2 * player->accel) {
            // top collision
            player->dstrect.y -= player->accel;
            collisionState->on_the_floor = true;
            collisionState->on_the_platform = true;
        }
    }

    else if (player->dstrect.x + 24 > platform->dstrect.x &&
             player->dstrect.x + 24 < platform->dstrect.x + 24) {
        if (player->dstrect.y + 24 > platform->dstrect.y &&
            player->dstrect.y + 24 < platform->dstrect.y + 2 * player->accel) {
            // top collision
            player->dstrect.y -= player->accel;
            collisionState->on_the_floor = true;
            collisionState->on_the_platform = true;
        }
    }

    else if (player->dstrect.x + 24 / 2 > platform->dstrect.x &&
             player->dstrect.x + 24 / 2 < platform->dstrect.x + 25) {
        if (player->dstrect.y + 24 > platform->dstrect.y &&
            player->dstrect.y + 24 < platform->dstrect.y + 2 * player->accel) {
            // top collision
            player->dstrect.y -= player->accel;
            collisionState->on_the_floor = true;
            collisionState->on_the_platform = true;
        }
    }
}

