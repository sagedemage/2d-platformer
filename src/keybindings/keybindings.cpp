#include "keybindings.hpp"

bool clickKeybindings(SDL_Event event, MotionState *motionState,
                      CollisionState *collisionState, SDL_Rect *dstrect,
                      int accel) {
    bool quit = false;
    // Click Keybindings
    switch (event.type) {
    case SDL_QUIT: // close button
        quit = true;
        break;
    case SDL_KEYDOWN: // key press
        if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            quit = true;
        }
        break;
    case SDL_KEYUP:
        if (event.key.keysym.scancode == SDL_SCANCODE_K &&
            collisionState->on_the_floor == true) {
            // Player jumps
            motionState->jump = true;
            collisionState->on_the_floor = false;
            collisionState->on_the_platform = false;
        } else if (event.key.keysym.scancode == SDL_SCANCODE_S &&
                   collisionState->on_the_platform == true &&
                   collisionState->on_the_floor == true) {
            // Player drops down from the platform
            dstrect->y += accel;
            collisionState->on_the_floor = false;
            collisionState->on_the_platform = false;
        }
        break;
    case SDL_CONTROLLERBUTTONDOWN: // controller button press
        if (event.cbutton.button == SDL_CONTROLLER_BUTTON_START) {
            quit = true;
        } else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A &&
                   collisionState->on_the_floor == true) {
            // Player jumps
            motionState->jump = true;
            collisionState->on_the_floor = false;
            collisionState->on_the_platform = false;
        } else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN &&
                   collisionState->on_the_platform == true &&
                   collisionState->on_the_floor == true) {
            // Player drops down from the platform
            dstrect->y += accel;
            collisionState->on_the_floor = false;
            collisionState->on_the_platform = false;
        }
        break;
    default:
        break;
    }
    return quit;
}

void holdKeybindings(Player *player, SDL_GameController *gamecontroller) {
    /* Hold Keybindings */
    // Get the snapshot of the current state of the keyboard
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    int left_dpad = SDL_GameControllerGetButton(
        gamecontroller, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    int right_dpad = SDL_GameControllerGetButton(
        gamecontroller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

    if (state[SDL_SCANCODE_A] == 1 || left_dpad == 1) {
        // move player left
        player->dstrect.x -= player->speed;
    } else if (state[SDL_SCANCODE_D] == 1 || right_dpad == 1) {
        // move player right
        player->dstrect.x += player->speed;
    }
}
