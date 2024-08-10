#ifndef KEYBINDINGS_H
#define KEYBINDINGS_H

#include "engine/entities.hpp"

bool clickKeybindings(SDL_Event event, MotionState *motionState, CollisionState *collisionState, SDL_Rect *dstrect, int accel);

void holdKeybindings(Player *player, SDL_GameController *gamecontroller);

#endif // KEYBINDINGS_H