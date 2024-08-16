#include "physics.hpp"

void Gravity(Player *player) { player->dstrect.y += player->accel; }

void JumpPhysics(Player *player, MotionState *motion_state) {
    /* Jump physics */
    if (motion_state->jump) {
        player->dstrect.y -= player->accel * 2;
        motion_state->jump_frames += 1;
    }

    if (motion_state->jump_frames == 15) {
        motion_state->jump = false;
        motion_state->jump_frames = 0;
    }
}
