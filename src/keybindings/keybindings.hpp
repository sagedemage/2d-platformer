#ifndef KEYBINDINGS_H
#define KEYBINDINGS_H

#include "engine/entities.hpp"

bool clickKeybindings(SDL_Event event, MotionState *motionState, CollisionState *collisionState);

void holdKeybindings(Player *player, SDL_GameController *gamecontroller);

#endif // KEYBINDINGS_H