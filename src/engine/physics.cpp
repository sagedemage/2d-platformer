#include "physics.hpp"

void gravity(Player *player) { player->dstrect.y += player->accel; }

void jumpPhysics(Player *player, MotionState *motionState) {
    /* Jump physics */
    if (motionState->jump == true) {
        player->dstrect.y -= player->accel * 2;
        motionState->jump_frames += 1;
    }

    if (motionState->jump_frames == 15) {
        motionState->jump = false;
        motionState->jump_frames = 0;
    }
}

void dropdownPhysics(Player *player, MotionState *motionState) {
    /* Drop physics */
    if (motionState->dropdown == true) {
        player->dstrect.y += player->accel;
        motionState->dropdown_frames += 1;
    }

    if (motionState->dropdown_frames == 2) {
        motionState->dropdown = false;
        motionState->dropdown_frames = 0;
    }
}
