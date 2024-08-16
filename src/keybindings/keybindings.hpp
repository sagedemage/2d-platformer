#ifndef KEYBINDINGS_HPP
#define KEYBINDINGS_HPP

#include "engine/entities.hpp"

bool ClickKeybindings(SDL_Event event, MotionState *motion_state,
                      CollisionState *collision_state, SDL_Rect *dstrect,
                      int accel);

void HoldKeybindings(Player *player, SDL_GameController *gamecontroller);

#endif  // KEYBINDINGS_HPP
