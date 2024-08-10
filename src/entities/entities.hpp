#ifndef ENTITIES_H
#define ENTITIES_H

typedef struct Player {
    SDL_Texture *PlayerTex; // player texture
    int speed;        // horizontal and vertical velocity
    int accel;        // horizonatal acceleration
    SDL_Rect srcrect;       // player source from the player spritesheet
    SDL_Rect dstrect; // player destination
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
