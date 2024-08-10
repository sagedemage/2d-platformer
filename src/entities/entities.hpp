#ifndef ENTITIES_H
#define ENTITIES_H

typedef struct CollisionState {
    bool on_the_floor;
    bool on_the_platform;
} CollisionState;

typedef struct MotionState {
    bool jump;
    bool dropdown;
    int jump_frames;
    int dropdown_frames;
} MotionState;

typedef struct Player {
    SDL_Texture *PlayerTex; // player texture
    int speed;        // horizontal and vertical velocity
    int accel;        // horizonatal acceleration
    SDL_Rect srcrect;       // player source from the player spritesheet
    SDL_Rect dstrect; // player destination
    CollisionState collisionState;
    MotionState motionState;
} Player;

typedef struct Wall {
    SDL_Texture *WallTex; // player texture
    SDL_Rect srcrect;       // player source from the player spritesheet
    SDL_Rect dstrect; // player destination
} Wall;

typedef struct Platform {
    SDL_Texture *PlatformTex; // player texture
    SDL_Rect srcrect;       // player source from the player spritesheet
    SDL_Rect dstrect; // player destination
} Platform;

#endif // ENTITIES_H
