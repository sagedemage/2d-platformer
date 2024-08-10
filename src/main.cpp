#include "entities/entities.hpp"

#include <stdexcept>

#define LEVEL_WIDTH 744  // 750
#define LEVEL_HEIGHT 504 // 500

/* Frames per second */
const int miliseconds = 1000;   // 1000 ms equals 1s
const int gameplay_frames = 60; // amount of frames per second

struct CollisionState {
    bool on_the_floor;
    bool on_the_platform;
};

struct MotionState {
    bool jump;
    bool drop;
    int jump_frames;
    int drop_frames;
};

void playerBoundary(Player *player);

void holdKeybindings(Player *player, SDL_GameController *gamecontroller);

void renderSprites(SDL_Renderer *rend, Player player, Wall walls[45],
                   Platform platforms[14]);

void setPosition(SDL_Rect *dstrect, int x, int y);

void gravity(Player *player);
void jumpPhysics(Player *player, MotionState *jumpState);

void playerPlatformCollision(Player *player, Platform *platform,
                             CollisionState *collisionState);

void playerWallCollisions(Player *player, Wall walls[45],
                          CollisionState *collisionState);

void freeAndCloseResources(SDL_Surface *PlayerSurf, SDL_Texture *PlayerTex,
                           SDL_Surface *WallSurf, SDL_Texture *WallTex,
                           Mix_Music *music, SDL_Renderer *rend,
                           SDL_Window *win, SDL_GameController *gamecontroller);

void playerObjectCollisions(Player *player, Wall walls[45],
                            Platform platforms[14],
                            CollisionState *collisionState);

//

int main() {
    /* Player Attributes */
    const int player_width = 24;
    const int player_height = 24;

    const int player_speed = 2;   // speed of player
    const int player_offset = 24; // gap between left corner of the window
    const int player_accel = 4;

    const int wall_width = 24;
    const int wall_height = 24;

    const int platform_width = 24;
    const int platform_height = 12;

    /* Mixer */
    const int music_volume = MIX_MAX_VOLUME;
    const int chunksize = 1024;

    /* Paths to the assets of the game */
    const char *player_path = "assets/art/player.png";
    const char *wall_path = "assets/art/wall.png";
    const char *music_path = "assets/music/downhill.ogg";
    const char *platform_path = "assets/art/platform.png";

    /* Initialize SDL, window, audio, and renderer */
    int sdl_status = SDL_Init(
        SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER); // Initialize SDL library

    if (sdl_status == -1) {
        std::string debug_msg = "SDL_Init: " + static_cast<std::string>(SDL_GetError());
        std::runtime_error(debug_msg.c_str());
    }

    SDL_GameController *gamecontroller =
        SDL_GameControllerOpen(0); // Open Game Controller

    // Create window
    SDL_Window *win =
        SDL_CreateWindow("2D Platformer", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, LEVEL_WIDTH, LEVEL_HEIGHT, 0);

    int open_audio_status =
        Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2,
                      chunksize); // Initialize SDL mixer

    if (open_audio_status == -1) {
        std::string debug_msg = "Mix_OpenAudio: " + static_cast<std::string>(Mix_GetError());
        std::runtime_error(debug_msg.c_str());
    }

    // Creates a renderer to render the images
    // * SDL_RENDERER_SOFTWARE starts the program using the CPU hardware
    // * SDL_RENDERER_ACCELERATED starts the program using the GPU hardware
    SDL_Renderer *rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetRenderDrawColor(rend, 134, 191, 255, 255);

    /* Loads images, music, and soundeffects */
    // Creates the asset that loads the image into main memory
    SDL_Surface *PlayerSurf = IMG_Load(player_path);

    if (PlayerSurf == NULL) {
        std::string debug_msg = "IMG_Load: " + static_cast<std::string>(IMG_GetError());
        std::runtime_error(debug_msg.c_str());
    }

    SDL_Surface *WallSurf = IMG_Load(wall_path);

    if (WallSurf == NULL) {
        std::string debug_msg = "IMG_Load: " + static_cast<std::string>(IMG_GetError());
        std::runtime_error(debug_msg.c_str());
    }

    SDL_Surface *PlatformSurf = IMG_Load(platform_path);

    if (PlatformSurf == NULL) {
        std::string debug_msg = "IMG_Load: " + static_cast<std::string>(IMG_GetError());
        std::runtime_error(debug_msg.c_str());
    }

    Mix_Music *music = Mix_LoadMUS(music_path);

    if (music == NULL) {
        std::string debug_msg = "Mix_LoadMUS: " + static_cast<std::string>(Mix_GetError());
        std::runtime_error(debug_msg.c_str());
    }

    // Loads images to our graphics hardware memory
    // Player
    SDL_Texture *PlayerTex = SDL_CreateTextureFromSurface(rend, PlayerSurf);
    SDL_Rect p_dstrect = {0 + player_offset,
                          LEVEL_HEIGHT - player_height - player_offset,
                          player_width, player_height};
    SDL_Rect p_srcrect = {0, 0, player_width, player_height};
    Player player;
    player.dstrect = p_dstrect;
    player.srcrect = p_srcrect;
    player.speed = player_speed;
    player.PlayerTex = PlayerTex;
    player.accel = player_accel;
    MotionState motionState;
    motionState.jump = false;
    motionState.jump_frames = 0;
    motionState.drop = false;
    motionState.drop_frames = 0;

    CollisionState collisionState;

    // Wall
    SDL_Texture *WallTex = SDL_CreateTextureFromSurface(rend, WallSurf);

    if (WallTex == NULL) {
        std::string debug_msg = "SDL_CreateTextureFromSurface: " + static_cast<std::string>(SDL_GetError());
        std::runtime_error(debug_msg.c_str());
    }

    SDL_Rect w_dstrect = {LEVEL_WIDTH - 200, LEVEL_HEIGHT - 200, wall_width,
                          wall_height};

    SDL_Rect w_srcrect = {0, 0, wall_width, wall_height};
    Wall wall;
    wall.dstrect = w_dstrect;
    wall.srcrect = w_srcrect;
    wall.WallTex = WallTex;

    // Platform
    SDL_Texture *PlatformTex = SDL_CreateTextureFromSurface(rend, PlatformSurf);

    if (PlatformTex == NULL) {
        std::string debug_msg = "SDL_CreateTextureFromSurface: " + static_cast<std::string>(SDL_GetError());
        std::runtime_error(debug_msg.c_str());
    }

    SDL_Rect pl_dstrect = {LEVEL_WIDTH - 200, LEVEL_HEIGHT - 200,
                           platform_width, platform_height};

    SDL_Rect pl_srcrect = {0, 0, platform_width, platform_height};
    Platform platform;
    platform.dstrect = pl_dstrect;
    platform.srcrect = pl_srcrect;
    platform.PlatformTex = PlatformTex;

    // Walls
    Wall walls[45] = {wall, wall, wall, wall, wall, wall, wall, wall, wall,
                      wall, wall, wall, wall, wall, wall, wall, wall, wall,
                      wall, wall, wall, wall, wall, wall, wall, wall, wall,
                      wall, wall, wall, wall, wall, wall, wall, wall, wall,
                      wall, wall, wall, wall, wall, wall, wall, wall, wall};

    // Platforms
    Platform platforms[14] = {
        platform, platform, platform, platform, platform, platform, platform,
        platform, platform, platform, platform, platform, platform, platform,
    };

    /* Map layout */
    // Set positions of the wall
    setPosition(&walls[0].dstrect, LEVEL_WIDTH - 24, LEVEL_HEIGHT - 24);
    setPosition(&walls[1].dstrect, LEVEL_WIDTH - 48, LEVEL_HEIGHT - 24);
    setPosition(&walls[2].dstrect, LEVEL_WIDTH - 72, LEVEL_HEIGHT - 24);
    setPosition(&walls[3].dstrect, LEVEL_WIDTH - 96, LEVEL_HEIGHT - 24);
    setPosition(&walls[4].dstrect, LEVEL_WIDTH - 120, LEVEL_HEIGHT - 24);
    setPosition(&walls[5].dstrect, LEVEL_WIDTH - 144, LEVEL_HEIGHT - 24);
    setPosition(&walls[6].dstrect, LEVEL_WIDTH - 168, LEVEL_HEIGHT - 24);
    setPosition(&walls[7].dstrect, LEVEL_WIDTH - 192, LEVEL_HEIGHT - 24);
    setPosition(&walls[8].dstrect, LEVEL_WIDTH - 216, LEVEL_HEIGHT - 24);
    setPosition(&walls[9].dstrect, LEVEL_WIDTH - 240, LEVEL_HEIGHT - 24);

    setPosition(&walls[10].dstrect, LEVEL_WIDTH - 264, LEVEL_HEIGHT - 24);
    setPosition(&walls[11].dstrect, LEVEL_WIDTH - 288, LEVEL_HEIGHT - 24);
    setPosition(&walls[12].dstrect, LEVEL_WIDTH - 312, LEVEL_HEIGHT - 24);
    setPosition(&walls[13].dstrect, LEVEL_WIDTH - 336, LEVEL_HEIGHT - 24);
    setPosition(&walls[14].dstrect, LEVEL_WIDTH - 360, LEVEL_HEIGHT - 24);
    setPosition(&walls[15].dstrect, LEVEL_WIDTH - 384, LEVEL_HEIGHT - 24);
    setPosition(&walls[16].dstrect, LEVEL_WIDTH - 408, LEVEL_HEIGHT - 24);
    setPosition(&walls[17].dstrect, LEVEL_WIDTH - 432, LEVEL_HEIGHT - 24);
    setPosition(&walls[18].dstrect, LEVEL_WIDTH - 456, LEVEL_HEIGHT - 24);
    setPosition(&walls[19].dstrect, LEVEL_WIDTH - 480, LEVEL_HEIGHT - 24);

    setPosition(&walls[20].dstrect, LEVEL_WIDTH - 504, LEVEL_HEIGHT - 24);
    setPosition(&walls[21].dstrect, LEVEL_WIDTH - 528, LEVEL_HEIGHT - 24);
    setPosition(&walls[22].dstrect, LEVEL_WIDTH - 552, LEVEL_HEIGHT - 24);
    setPosition(&walls[23].dstrect, LEVEL_WIDTH - 576, LEVEL_HEIGHT - 24);
    setPosition(&walls[24].dstrect, LEVEL_WIDTH - 600, LEVEL_HEIGHT - 24);
    setPosition(&walls[25].dstrect, LEVEL_WIDTH - 624, LEVEL_HEIGHT - 24);
    setPosition(&walls[26].dstrect, LEVEL_WIDTH - 648, LEVEL_HEIGHT - 24);
    setPosition(&walls[27].dstrect, LEVEL_WIDTH - 672, LEVEL_HEIGHT - 24);
    setPosition(&walls[28].dstrect, LEVEL_WIDTH - 696, LEVEL_HEIGHT - 24);
    setPosition(&walls[29].dstrect, LEVEL_WIDTH - 720, LEVEL_HEIGHT - 24);

    setPosition(&walls[30].dstrect, LEVEL_WIDTH - 744, LEVEL_HEIGHT - 24);
    setPosition(&walls[31].dstrect, LEVEL_WIDTH - 216, LEVEL_HEIGHT - 72);
    setPosition(&walls[32].dstrect, LEVEL_WIDTH - 240, LEVEL_HEIGHT - 72);
    setPosition(&walls[33].dstrect, LEVEL_WIDTH - 264, LEVEL_HEIGHT - 72);
    setPosition(&walls[34].dstrect, LEVEL_WIDTH - 288, LEVEL_HEIGHT - 72);
    setPosition(&walls[35].dstrect, LEVEL_WIDTH - 312, LEVEL_HEIGHT - 72);
    setPosition(&walls[36].dstrect, LEVEL_WIDTH - 336, LEVEL_HEIGHT - 72);
    setPosition(&walls[37].dstrect, LEVEL_WIDTH - 360, LEVEL_HEIGHT - 72);
    setPosition(&walls[38].dstrect, LEVEL_WIDTH - 384, LEVEL_HEIGHT - 72);
    setPosition(&walls[39].dstrect, LEVEL_WIDTH - 408, LEVEL_HEIGHT - 72);

    setPosition(&walls[40].dstrect, LEVEL_WIDTH - 432, LEVEL_HEIGHT - 72);
    setPosition(&walls[41].dstrect, LEVEL_WIDTH - 456, LEVEL_HEIGHT - 72);
    setPosition(&walls[42].dstrect, LEVEL_WIDTH - 480, LEVEL_HEIGHT - 72);
    setPosition(&walls[43].dstrect, LEVEL_WIDTH - 504, LEVEL_HEIGHT - 72);
    setPosition(&walls[44].dstrect, LEVEL_WIDTH - 528, LEVEL_HEIGHT - 72);

    // Set the positions of the platforms
    setPosition(&platforms[0].dstrect, LEVEL_WIDTH - 216, LEVEL_HEIGHT - 120);
    setPosition(&platforms[1].dstrect, LEVEL_WIDTH - 240, LEVEL_HEIGHT - 120);
    setPosition(&platforms[2].dstrect, LEVEL_WIDTH - 264, LEVEL_HEIGHT - 120);
    setPosition(&platforms[3].dstrect, LEVEL_WIDTH - 288, LEVEL_HEIGHT - 120);
    setPosition(&platforms[4].dstrect, LEVEL_WIDTH - 312, LEVEL_HEIGHT - 120);
    setPosition(&platforms[5].dstrect, LEVEL_WIDTH - 336, LEVEL_HEIGHT - 120);
    setPosition(&platforms[6].dstrect, LEVEL_WIDTH - 360, LEVEL_HEIGHT - 120);
    setPosition(&platforms[7].dstrect, LEVEL_WIDTH - 384, LEVEL_HEIGHT - 120);
    setPosition(&platforms[8].dstrect, LEVEL_WIDTH - 408, LEVEL_HEIGHT - 120);
    setPosition(&platforms[9].dstrect, LEVEL_WIDTH - 432, LEVEL_HEIGHT - 120);
    setPosition(&platforms[10].dstrect, LEVEL_WIDTH - 456, LEVEL_HEIGHT - 120);
    setPosition(&platforms[11].dstrect, LEVEL_WIDTH - 480, LEVEL_HEIGHT - 120);
    setPosition(&platforms[12].dstrect, LEVEL_WIDTH - 504, LEVEL_HEIGHT - 120);
    setPosition(&platforms[13].dstrect, LEVEL_WIDTH - 528, LEVEL_HEIGHT - 120);

    Mix_VolumeMusic(music_volume); // Adjust music volume

    int player_music_status =
        Mix_PlayMusic(music, -1); // Start background music (-1 means infinity)

    if (player_music_status == -1) {
        std::string debug_msg = "Mix_PlayMusic: " + static_cast<std::string>(Mix_GetError());
        std::runtime_error(debug_msg.c_str());
    }

    /* Gameplay Loop */
    // GamePlay(rend, player, gamecontroller); // Start movement and physics
    bool quit = false; // gameplay loop switch

#pragma unroll
    while (!quit) { // gameplay loop
        /* Keybindings */
        // quit = ClickKeybindings(quit); // Click

        /* Click Key Bindings */
        SDL_Event event; // Event handling

#pragma unroll
        while (SDL_PollEvent(&event) == 1) { // Events management
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
                if (event.key.keysym.scancode == SDL_SCANCODE_UP &&
                    collisionState.on_the_floor == true) {
                    // Player jumps
                    motionState.jump = true;
                    collisionState.on_the_floor = false;
                    collisionState.on_the_platform = false;
                } else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN &&
                           collisionState.on_the_platform == true &&
                           collisionState.on_the_floor == true) {
                    // Player jumps
                    motionState.drop = true;
                    collisionState.on_the_floor = false;
                    collisionState.on_the_platform = false;
                }
                break;
            case SDL_CONTROLLERBUTTONDOWN: // controller button press
                if (event.cbutton.button == SDL_CONTROLLER_BUTTON_START) {
                    quit = true;
                }
                break;
            default:
                break;
            }
        }

        /* Hold Keybindings */
        holdKeybindings(&player, gamecontroller);

        /* Player boundaries */
        playerBoundary(&player);

        /* Render sprites */
        renderSprites(rend, player, walls, platforms);

        /* Gravity */
        gravity(&player);

        /* Jump physics */
        jumpPhysics(&player, &motionState);

        /* Drop physics */
        if (motionState.drop == true) {
            player.dstrect.y += player.accel;
            motionState.drop_frames += 1;
        }

        if (motionState.drop_frames == 5) {
            motionState.drop = false;
            motionState.drop_frames = 0;
        }

        /* Player wall collisons */
        playerObjectCollisions(&player, walls, platforms, &collisionState);
    }

    /* Free resources and close SDL and SDL mixer */
    freeAndCloseResources(PlayerSurf, PlayerTex, WallSurf, WallTex, music, rend,
                          win, gamecontroller);

    return 0;
}

void playerBoundary(Player *player) {
    /* Player boundaries */
    // left boundary
    if (player->dstrect.x < 0) {
        player->dstrect.x = 0;
    }
    // right boundary
    if (player->dstrect.x + player->dstrect.w > LEVEL_WIDTH) {
        player->dstrect.x = LEVEL_WIDTH - player->dstrect.w;
    }
    // bottom boundary
    if (player->dstrect.y + player->dstrect.h > LEVEL_HEIGHT) {
        player->dstrect.y = LEVEL_HEIGHT - player->dstrect.h;
    }
    // top boundary
    if (player->dstrect.y < 0) {
        player->dstrect.y = 0;
    }
}

void holdKeybindings(Player *player, SDL_GameController *gamecontroller) {
    /* Hold Keybindings */
    // Get the snapshot of the current state of the keyboard
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    int left_dpad = SDL_GameControllerGetButton(
        gamecontroller, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    int right_dpad = SDL_GameControllerGetButton(
        gamecontroller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
    /*int down_dpad = SDL_GameControllerGetButton(
        gamecontroller, SDL_CONTROLLER_BUTTON_DPAD_DOWN);*/
    int up_dpad = SDL_GameControllerGetButton(gamecontroller,
                                              SDL_CONTROLLER_BUTTON_DPAD_UP);

    if (state[SDL_SCANCODE_LEFT] == 1 || left_dpad == 1) { // move player left
        player->dstrect.x -= player->speed;
    } else if (state[SDL_SCANCODE_RIGHT] == 1 ||
               right_dpad == 1) { // move player right
        player->dstrect.x += player->speed;

    } else if (state[SDL_SCANCODE_UP] == 1 || up_dpad == 1) { // move player up
        // player->dstrect.y -= player->accel*8;
    }

    /*else if (state[SDL_SCANCODE_DOWN] == 1 ||
               down_dpad == 1) { // move player down
        player->dstrect.y += player->speed;
    }*/
}

void renderSprites(SDL_Renderer *rend, Player player, Wall walls[45],
                   Platform platforms[14]) {
    /* Render sprites */
    SDL_RenderClear(rend);

    // Render Walls
    SDL_RenderCopy(rend, walls[0].WallTex, &walls[0].srcrect,
                   &walls[0].dstrect);
    SDL_RenderCopy(rend, walls[1].WallTex, &walls[1].srcrect,
                   &walls[1].dstrect);
    SDL_RenderCopy(rend, walls[2].WallTex, &walls[2].srcrect,
                   &walls[2].dstrect);
    SDL_RenderCopy(rend, walls[3].WallTex, &walls[3].srcrect,
                   &walls[3].dstrect);
    SDL_RenderCopy(rend, walls[4].WallTex, &walls[4].srcrect,
                   &walls[4].dstrect);
    SDL_RenderCopy(rend, walls[5].WallTex, &walls[5].srcrect,
                   &walls[5].dstrect);
    SDL_RenderCopy(rend, walls[6].WallTex, &walls[6].srcrect,
                   &walls[6].dstrect);
    SDL_RenderCopy(rend, walls[7].WallTex, &walls[7].srcrect,
                   &walls[7].dstrect);
    SDL_RenderCopy(rend, walls[8].WallTex, &walls[8].srcrect,
                   &walls[8].dstrect);
    SDL_RenderCopy(rend, walls[9].WallTex, &walls[9].srcrect,
                   &walls[9].dstrect);

    SDL_RenderCopy(rend, walls[10].WallTex, &walls[10].srcrect,
                   &walls[10].dstrect);
    SDL_RenderCopy(rend, walls[11].WallTex, &walls[11].srcrect,
                   &walls[11].dstrect);
    SDL_RenderCopy(rend, walls[12].WallTex, &walls[12].srcrect,
                   &walls[12].dstrect);
    SDL_RenderCopy(rend, walls[13].WallTex, &walls[13].srcrect,
                   &walls[13].dstrect);
    SDL_RenderCopy(rend, walls[14].WallTex, &walls[14].srcrect,
                   &walls[14].dstrect);
    SDL_RenderCopy(rend, walls[15].WallTex, &walls[15].srcrect,
                   &walls[15].dstrect);
    SDL_RenderCopy(rend, walls[16].WallTex, &walls[16].srcrect,
                   &walls[16].dstrect);
    SDL_RenderCopy(rend, walls[17].WallTex, &walls[17].srcrect,
                   &walls[17].dstrect);
    SDL_RenderCopy(rend, walls[18].WallTex, &walls[18].srcrect,
                   &walls[18].dstrect);
    SDL_RenderCopy(rend, walls[19].WallTex, &walls[19].srcrect,
                   &walls[19].dstrect);

    SDL_RenderCopy(rend, walls[20].WallTex, &walls[20].srcrect,
                   &walls[20].dstrect);
    SDL_RenderCopy(rend, walls[21].WallTex, &walls[21].srcrect,
                   &walls[21].dstrect);
    SDL_RenderCopy(rend, walls[22].WallTex, &walls[22].srcrect,
                   &walls[22].dstrect);
    SDL_RenderCopy(rend, walls[23].WallTex, &walls[23].srcrect,
                   &walls[23].dstrect);
    SDL_RenderCopy(rend, walls[24].WallTex, &walls[24].srcrect,
                   &walls[24].dstrect);
    SDL_RenderCopy(rend, walls[25].WallTex, &walls[25].srcrect,
                   &walls[25].dstrect);
    SDL_RenderCopy(rend, walls[26].WallTex, &walls[26].srcrect,
                   &walls[26].dstrect);
    SDL_RenderCopy(rend, walls[27].WallTex, &walls[27].srcrect,
                   &walls[27].dstrect);
    SDL_RenderCopy(rend, walls[28].WallTex, &walls[28].srcrect,
                   &walls[28].dstrect);
    SDL_RenderCopy(rend, walls[29].WallTex, &walls[29].srcrect,
                   &walls[29].dstrect);

    SDL_RenderCopy(rend, walls[30].WallTex, &walls[30].srcrect,
                   &walls[30].dstrect);
    SDL_RenderCopy(rend, walls[31].WallTex, &walls[31].srcrect,
                   &walls[31].dstrect);
    SDL_RenderCopy(rend, walls[32].WallTex, &walls[32].srcrect,
                   &walls[32].dstrect);
    SDL_RenderCopy(rend, walls[33].WallTex, &walls[33].srcrect,
                   &walls[33].dstrect);
    SDL_RenderCopy(rend, walls[34].WallTex, &walls[34].srcrect,
                   &walls[34].dstrect);
    SDL_RenderCopy(rend, walls[35].WallTex, &walls[35].srcrect,
                   &walls[35].dstrect);
    SDL_RenderCopy(rend, walls[36].WallTex, &walls[36].srcrect,
                   &walls[36].dstrect);
    SDL_RenderCopy(rend, walls[37].WallTex, &walls[37].srcrect,
                   &walls[37].dstrect);
    SDL_RenderCopy(rend, walls[38].WallTex, &walls[38].srcrect,
                   &walls[38].dstrect);
    SDL_RenderCopy(rend, walls[39].WallTex, &walls[39].srcrect,
                   &walls[39].dstrect);
    SDL_RenderCopy(rend, walls[40].WallTex, &walls[40].srcrect,
                   &walls[40].dstrect);
    SDL_RenderCopy(rend, walls[41].WallTex, &walls[41].srcrect,
                   &walls[41].dstrect);
    SDL_RenderCopy(rend, walls[42].WallTex, &walls[42].srcrect,
                   &walls[42].dstrect);
    SDL_RenderCopy(rend, walls[43].WallTex, &walls[43].srcrect,
                   &walls[43].dstrect);
    SDL_RenderCopy(rend, walls[44].WallTex, &walls[44].srcrect,
                   &walls[44].dstrect);

    // Render platforms
    SDL_RenderCopy(rend, platforms[0].PlatformTex, &platforms[0].srcrect,
                   &platforms[0].dstrect);
    SDL_RenderCopy(rend, platforms[1].PlatformTex, &platforms[1].srcrect,
                   &platforms[1].dstrect);
    SDL_RenderCopy(rend, platforms[2].PlatformTex, &platforms[2].srcrect,
                   &platforms[2].dstrect);
    SDL_RenderCopy(rend, platforms[3].PlatformTex, &platforms[3].srcrect,
                   &platforms[3].dstrect);
    SDL_RenderCopy(rend, platforms[4].PlatformTex, &platforms[4].srcrect,
                   &platforms[4].dstrect);
    SDL_RenderCopy(rend, platforms[5].PlatformTex, &platforms[5].srcrect,
                   &platforms[5].dstrect);
    SDL_RenderCopy(rend, platforms[6].PlatformTex, &platforms[6].srcrect,
                   &platforms[6].dstrect);
    SDL_RenderCopy(rend, platforms[7].PlatformTex, &platforms[7].srcrect,
                   &platforms[7].dstrect);
    SDL_RenderCopy(rend, platforms[8].PlatformTex, &platforms[8].srcrect,
                   &platforms[8].dstrect);
    SDL_RenderCopy(rend, platforms[9].PlatformTex, &platforms[9].srcrect,
                   &platforms[9].dstrect);
    SDL_RenderCopy(rend, platforms[10].PlatformTex, &platforms[10].srcrect,
                   &platforms[10].dstrect);
    SDL_RenderCopy(rend, platforms[11].PlatformTex, &platforms[11].srcrect,
                   &platforms[11].dstrect);
    SDL_RenderCopy(rend, platforms[12].PlatformTex, &platforms[12].srcrect,
                   &platforms[12].dstrect);
    SDL_RenderCopy(rend, platforms[13].PlatformTex, &platforms[13].srcrect,
                   &platforms[13].dstrect);
    SDL_RenderCopy(rend, platforms[14].PlatformTex, &platforms[14].srcrect,
                   &platforms[14].dstrect);

    SDL_RenderCopy(rend, player.PlayerTex, &player.srcrect, &player.dstrect);
    SDL_RenderPresent(rend); // Triggers double buffers for multiple rendering
    SDL_Delay(miliseconds / gameplay_frames); // Calculates to 60 fps
}

void setPosition(SDL_Rect *dstrect, int x, int y) {
    dstrect->x = x;
    dstrect->y = y;
}

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

void playerWallCollision(Player *player, Wall *wall,
                         CollisionState *collisionState) {
    int offset = 5;
    /* X Axis Collision */
    if (player->dstrect.y > wall->dstrect.y + offset &&
        player->dstrect.y < wall->dstrect.y + 24 - offset) {
        if (player->dstrect.x + 24 > wall->dstrect.x &&
            player->dstrect.x + 24 < wall->dstrect.x + 24) {
            // left collision
            player->dstrect.x -= player->speed;
        } else if (player->dstrect.x < wall->dstrect.x + 24 &&
                   player->dstrect.x > wall->dstrect.x) {
            // right collision
            player->dstrect.x += player->speed;
        }
    }

    else if (player->dstrect.y + 24 > wall->dstrect.y + offset &&
             player->dstrect.y + 24 < wall->dstrect.y + 24 - offset) {
        if (player->dstrect.x + 24 > wall->dstrect.x &&
            player->dstrect.x + 24 < wall->dstrect.x + 24) {
            // left collision
            player->dstrect.x -= player->speed;
        } else if (player->dstrect.x < wall->dstrect.x + 24 &&
                   player->dstrect.x > wall->dstrect.x) {
            // right collision
            player->dstrect.x += player->speed;
        }
    }

    else if (player->dstrect.y + 24 / 2 > wall->dstrect.y &&
             player->dstrect.y + 24 / 2 < wall->dstrect.y + 24) {
        if (player->dstrect.x + 24 > wall->dstrect.x &&
            player->dstrect.x + 24 < wall->dstrect.x + 24) {
            // left collision
            player->dstrect.x -= player->speed;
        } else if (player->dstrect.x < wall->dstrect.x + 24 &&
                   player->dstrect.x > wall->dstrect.x) {
            // right collision
            player->dstrect.x += player->speed;
        }
    }

    /* Y Axis Collision */
    if (player->dstrect.x > wall->dstrect.x &&
        player->dstrect.x < wall->dstrect.x + 24) {
        if (player->dstrect.y + 24 > wall->dstrect.y &&
            player->dstrect.y + 24 < wall->dstrect.y + 24) {
            // top collision
            player->dstrect.y -= player->accel;
            collisionState->on_the_floor = true;
        } else if (player->dstrect.y < wall->dstrect.y + 24 &&
                   player->dstrect.y > wall->dstrect.y) {
            // bottom collision
            player->dstrect.y += player->accel;
        }
    }

    else if (player->dstrect.x + 24 > wall->dstrect.x &&
             player->dstrect.x + 24 < wall->dstrect.x + 24) {
        if (player->dstrect.y + 24 > wall->dstrect.y &&
            player->dstrect.y + 24 < wall->dstrect.y + 24) {
            // top collision
            player->dstrect.y -= player->accel;
            collisionState->on_the_floor = true;
        } else if (player->dstrect.y < wall->dstrect.y + 24 &&
                   player->dstrect.y > wall->dstrect.y) {
            // bottom collision
            player->dstrect.y += player->accel;
        }
    }

    else if (player->dstrect.x + 24 / 2 > wall->dstrect.x &&
             player->dstrect.x + 24 / 2 < wall->dstrect.x + 24) {
        if (player->dstrect.y + 24 > wall->dstrect.y &&
            player->dstrect.y + 24 < wall->dstrect.y + 24) {
            // top collision
            player->dstrect.y -= player->accel;
            collisionState->on_the_floor = true;
        } else if (player->dstrect.y < wall->dstrect.y + 24 &&
                   player->dstrect.y > wall->dstrect.y) {
            // bottom collision
            player->dstrect.y += player->accel;
        }
    }
}

void playerPlatformCollision(Player *player, Platform *platform,
                             CollisionState *collisionState) {
    /* Y Axis Collision */
    if (player->dstrect.x > platform->dstrect.x &&
        player->dstrect.x < platform->dstrect.x + 24) {
        if (player->dstrect.y + 24 > platform->dstrect.y &&
            player->dstrect.y + 24 < platform->dstrect.y + 2 * player->accel) {
            // top collision
            player->dstrect.y -= player->accel;
            collisionState->on_the_floor = true;
            collisionState->on_the_platform = true;
        }
    }

    else if (player->dstrect.x + 24 > platform->dstrect.x &&
             player->dstrect.x + 24 < platform->dstrect.x + 24) {
        if (player->dstrect.y + 24 > platform->dstrect.y &&
            player->dstrect.y + 24 < platform->dstrect.y + 2 * player->accel) {
            // top collision
            player->dstrect.y -= player->accel;
            collisionState->on_the_floor = true;
            collisionState->on_the_platform = true;
        }
    }

    else if (player->dstrect.x + 24 / 2 > platform->dstrect.x &&
             player->dstrect.x + 24 / 2 < platform->dstrect.x + 25) {
        if (player->dstrect.y + 24 > platform->dstrect.y &&
            player->dstrect.y + 24 < platform->dstrect.y + 2 * player->accel) {
            // top collision
            player->dstrect.y -= player->accel;
            collisionState->on_the_floor = true;
            collisionState->on_the_platform = true;
        }
    }
}

void playerObjectCollisions(Player *player, Wall walls[45],
                            Platform platforms[14],
                            CollisionState *collisionState) {
    /* Player wall collisons */
    playerWallCollision(player, &walls[0], collisionState);
    playerWallCollision(player, &walls[1], collisionState);
    playerWallCollision(player, &walls[2], collisionState);
    playerWallCollision(player, &walls[3], collisionState);
    playerWallCollision(player, &walls[4], collisionState);
    playerWallCollision(player, &walls[5], collisionState);
    playerWallCollision(player, &walls[6], collisionState);
    playerWallCollision(player, &walls[7], collisionState);
    playerWallCollision(player, &walls[8], collisionState);
    playerWallCollision(player, &walls[9], collisionState);

    playerWallCollision(player, &walls[10], collisionState);
    playerWallCollision(player, &walls[11], collisionState);
    playerWallCollision(player, &walls[12], collisionState);
    playerWallCollision(player, &walls[13], collisionState);
    playerWallCollision(player, &walls[14], collisionState);
    playerWallCollision(player, &walls[15], collisionState);
    playerWallCollision(player, &walls[16], collisionState);
    playerWallCollision(player, &walls[17], collisionState);
    playerWallCollision(player, &walls[18], collisionState);
    playerWallCollision(player, &walls[19], collisionState);

    playerWallCollision(player, &walls[20], collisionState);
    playerWallCollision(player, &walls[21], collisionState);
    playerWallCollision(player, &walls[22], collisionState);
    playerWallCollision(player, &walls[23], collisionState);
    playerWallCollision(player, &walls[24], collisionState);
    playerWallCollision(player, &walls[25], collisionState);
    playerWallCollision(player, &walls[26], collisionState);
    playerWallCollision(player, &walls[27], collisionState);
    playerWallCollision(player, &walls[28], collisionState);
    playerWallCollision(player, &walls[29], collisionState);

    playerWallCollision(player, &walls[30], collisionState);
    playerWallCollision(player, &walls[31], collisionState);
    playerWallCollision(player, &walls[32], collisionState);
    playerWallCollision(player, &walls[33], collisionState);
    playerWallCollision(player, &walls[34], collisionState);
    playerWallCollision(player, &walls[35], collisionState);
    playerWallCollision(player, &walls[36], collisionState);
    playerWallCollision(player, &walls[37], collisionState);
    playerWallCollision(player, &walls[38], collisionState);
    playerWallCollision(player, &walls[39], collisionState);

    playerWallCollision(player, &walls[40], collisionState);
    playerWallCollision(player, &walls[41], collisionState);
    playerWallCollision(player, &walls[42], collisionState);
    playerWallCollision(player, &walls[43], collisionState);
    playerWallCollision(player, &walls[44], collisionState);

    /* Player PLatform Collisions */
    playerPlatformCollision(player, &platforms[0], collisionState);
    playerPlatformCollision(player, &platforms[1], collisionState);
    playerPlatformCollision(player, &platforms[2], collisionState);
    playerPlatformCollision(player, &platforms[3], collisionState);
    playerPlatformCollision(player, &platforms[4], collisionState);
    playerPlatformCollision(player, &platforms[5], collisionState);
    playerPlatformCollision(player, &platforms[6], collisionState);
    playerPlatformCollision(player, &platforms[7], collisionState);
    playerPlatformCollision(player, &platforms[8], collisionState);
    playerPlatformCollision(player, &platforms[9], collisionState);
    playerPlatformCollision(player, &platforms[10], collisionState);
    playerPlatformCollision(player, &platforms[11], collisionState);
    playerPlatformCollision(player, &platforms[12], collisionState);
    playerPlatformCollision(player, &platforms[13], collisionState);
}

void freeAndCloseResources(SDL_Surface *PlayerSurf, SDL_Texture *PlayerTex,
                           SDL_Surface *WallSurf, SDL_Texture *WallTex,
                           Mix_Music *music, SDL_Renderer *rend,
                           SDL_Window *win,
                           SDL_GameController *gamecontroller) {
    /* Free resources and close SDL and SDL mixer */
    Mix_FreeMusic(music); // Free the music

    // Deallocate textues and surfaces
    SDL_FreeSurface(PlayerSurf);   // Deallocate player and scene surfaces
    SDL_DestroyTexture(PlayerTex); // Destroy scene and player textures
    SDL_FreeSurface(WallSurf);     // Deallocate player and scene surfaces
    SDL_DestroyTexture(WallTex);   // Destroy scene and player textures

    // Close Game Controller
    SDL_GameControllerClose(gamecontroller);

    SDL_DestroyRenderer(rend); // Destroy renderer
    SDL_DestroyWindow(win);    // Destroy window
    Mix_CloseAudio();          // Close Audio
    IMG_Quit();                // Close Image
    SDL_Quit();                // Quit SDL subsystems
}
