#ifndef ENTITIES_HPP
#define ENTITIES_HPP

typedef struct CollisionState {
    bool on_the_floor;
    bool on_the_platform;
} CollisionState;

typedef struct MotionState {
    bool jump;
    int jump_frames;
} MotionState;

typedef struct Player {
    SDL_Texture *texture;  // player texture
    int speed;             // horizontal and vertical velocity
    int accel;             // horizonatal acceleration
    SDL_Rect srcrect;      // player source from the player spritesheet
    SDL_Rect dstrect;      // player destination
    CollisionState collision_state;
    MotionState motion_state;
} Player;

typedef struct Block {
    SDL_Texture *texture;  // player texture
    SDL_Rect srcrect;      // player source from the player spritesheet
    SDL_Rect dstrect;      // player destination
} Block;

typedef struct Platform {
    SDL_Texture *texture;  // player texture
    SDL_Rect srcrect;      // player source from the player spritesheet
    SDL_Rect dstrect;      // player destination
} Platform;

typedef struct Background {
    SDL_Texture *texture;  // player texture
    SDL_Rect srcrect;      // player source from the player spritesheet
    SDL_Rect dstrect;      // player destination
} Background;

#endif  // ENTITIES_HPP
