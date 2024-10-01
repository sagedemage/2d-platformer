#include "collision.hpp"

void PlayerBlockCollision(Player *player, Block *block,
                         CollisionState *collision_state) {
    const int offset = 5;

    const int p_width = player->dstrect.w;
    const int p_height = player->dstrect.h;

    const int w_width = block->dstrect.w;
    const int w_height = block->dstrect.h;

    /* X Axis Collision */
    if (player->dstrect.y > block->dstrect.y + offset &&
        player->dstrect.y < block->dstrect.y + w_height - offset) {
        if (player->dstrect.x + p_width > block->dstrect.x &&
            player->dstrect.x + p_width < block->dstrect.x + w_width) {
            // left collision
            player->dstrect.x -= player->speed;
        } else if (player->dstrect.x < block->dstrect.x + w_width &&
                   player->dstrect.x > block->dstrect.x) {
            // right collision
            player->dstrect.x += player->speed;
        }
    }

    else if (player->dstrect.y + p_height > block->dstrect.y + offset &&
             player->dstrect.y + p_height <
                 block->dstrect.y + w_height - offset) {
        if (player->dstrect.x + p_width > block->dstrect.x &&
            player->dstrect.x + p_width < block->dstrect.x + w_width) {
            // left collision
            player->dstrect.x -= player->speed;
        } else if (player->dstrect.x < block->dstrect.x + w_width &&
                   player->dstrect.x > block->dstrect.x) {
            // right collision
            player->dstrect.x += player->speed;
        }
    }

    else if (player->dstrect.y + p_height / 2 > block->dstrect.y &&
             player->dstrect.y + p_height / 2 < block->dstrect.y + w_height) {
        if (player->dstrect.x + p_width > block->dstrect.x &&
            player->dstrect.x + p_width < block->dstrect.x + w_width) {
            // left collision
            player->dstrect.x -= player->speed;
        } else if (player->dstrect.x < block->dstrect.x + w_width &&
                   player->dstrect.x > block->dstrect.x) {
            // right collision
            player->dstrect.x += player->speed;
        }
    }

    /* Y Axis Collision */
    if (player->dstrect.x > block->dstrect.x &&
        player->dstrect.x < block->dstrect.x + w_width) {
        if (player->dstrect.y + p_height > block->dstrect.y &&
            player->dstrect.y + p_height < block->dstrect.y + w_height) {
            // top collision
            player->dstrect.y -= player->accel;
            collision_state->on_the_floor = true;
        } else if (player->dstrect.y < block->dstrect.y + w_height &&
                   player->dstrect.y > block->dstrect.y) {
            // bottom collision
            player->dstrect.y += player->accel;
        }
    }

    else if (player->dstrect.x + p_width > block->dstrect.x &&
             player->dstrect.x + p_width < block->dstrect.x + w_width) {
        if (player->dstrect.y + p_height > block->dstrect.y &&
            player->dstrect.y + p_height < block->dstrect.y + w_height) {
            // top collision
            player->dstrect.y -= player->accel;
            collision_state->on_the_floor = true;
        } else if (player->dstrect.y < block->dstrect.y + w_height &&
                   player->dstrect.y > block->dstrect.y) {
            // bottom collision
            player->dstrect.y += player->accel;
        }
    }

    else if (player->dstrect.x + p_width / 2 > block->dstrect.x &&
             player->dstrect.x + p_width / 2 < block->dstrect.x + w_width) {
        if (player->dstrect.y + p_height > block->dstrect.y &&
            player->dstrect.y + p_height < block->dstrect.y + w_height) {
            // top collision
            player->dstrect.y -= player->accel;
            collision_state->on_the_floor = true;
        } else if (player->dstrect.y < block->dstrect.y + w_height &&
                   player->dstrect.y > block->dstrect.y) {
            // bottom collision
            player->dstrect.y += player->accel;
        }
    }
}

void PlayerPlatformCollision(Player *player, Platform *platform,
                             CollisionState *collision_state) {
    const int p_width = player->dstrect.w;
    const int p_height = player->dstrect.h;

    const int pl_width = platform->dstrect.w;

    /* Y Axis Collision */
    if (player->dstrect.x > platform->dstrect.x &&
        player->dstrect.x < platform->dstrect.x + pl_width) {
        if (player->dstrect.y + p_height > platform->dstrect.y &&
            player->dstrect.y + p_height <
                platform->dstrect.y + 2 * player->accel) {
            // top collision
            player->dstrect.y -= player->accel;
            collision_state->on_the_floor = true;
            collision_state->on_the_platform = true;
        }
    }

    else if (player->dstrect.x + p_width > platform->dstrect.x &&
             player->dstrect.x + p_width < platform->dstrect.x + pl_width) {
        if (player->dstrect.y + p_height > platform->dstrect.y &&
            player->dstrect.y + p_height <
                platform->dstrect.y + 2 * player->accel) {
            // top collision
            player->dstrect.y -= player->accel;
            collision_state->on_the_floor = true;
            collision_state->on_the_platform = true;
        }
    }

    else if (player->dstrect.x + p_width / 2 > platform->dstrect.x &&
             player->dstrect.x + p_width / 2 < platform->dstrect.x + pl_width) {
        if (player->dstrect.y + p_height > platform->dstrect.y &&
            player->dstrect.y + p_height <
                platform->dstrect.y + 2 * player->accel) {
            // top collision
            player->dstrect.y -= player->accel;
            collision_state->on_the_floor = true;
            collision_state->on_the_platform = true;
        }
    }
}
