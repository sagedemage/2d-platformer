#include <array>
#include <iostream>

#include "engine/collision.hpp"
#include "engine/entities.hpp"
#include "engine/physics.hpp"
#include "keybindings/keybindings.hpp"

constexpr int LEVEL_WIDTH = 744;   // 750
constexpr int LEVEL_HEIGHT = 504;  // 500

struct Coord2D {
    int x;
    int y;
};

void PlayerBoundary(Player *player);

void RenderSprites(SDL_Renderer *rend, Player player,
                   std::array<Wall, 45> walls,
                   std::array<Platform, 14> platforms, Background background);

void SetPosition(SDL_Rect *dstrect, Coord2D pos);

void FreeAndCloseResources(SDL_Surface *player_surf, SDL_Texture *player_tex,
                           SDL_Surface *wall_surf, SDL_Texture *wall_tex,
                           Mix_Music *music, SDL_Renderer *rend,
                           SDL_Window *win, SDL_GameController *gamecontroller);

void PlayerObjectCollisions(Player *player, std::array<Wall, 45> walls,
                            std::array<Platform, 14> platforms,
                            CollisionState *collision_state);

int main() {
    // Player Attributes
    const int player_width = 24;
    const int player_height = 24;
    const int player_speed = 2;    // speed of player
    const int player_offset = 24;  // gap between left corner of the window
    const int player_accel = 4;

    // Wall dimensions
    const int wall_width = 24;
    const int wall_height = 24;

    const int wall_source_width = 512;
    const int wall_source_height = 512;

    // Platform dimensions
    const int platform_width = 24;
    const int platform_height = 24;

    const int platform_source_width = 512;
    const int platform_source_height = 512;

    // Background dimensions
    const int background_width = LEVEL_WIDTH;
    const int background_height = LEVEL_HEIGHT;

    const int background_source_width = 1536;
    const int background_source_height = 1024;

    /* Mixer */
    const int music_volume = MIX_MAX_VOLUME / 2;
    const int chunksize = 1024;

    /* Paths to the assets of the game */
    const char *player_path = "assets/player/player.png";
    const char *wall_path = "assets/tiles/wall.png";
    const char *music_path = "assets/music/downhill.ogg";
    const char *platform_path = "assets/tiles/platform.png";
    const char *background_path = "assets/background/background.png";

    /* Initialize SDL, window, audio, and renderer */
    int sdl_status = SDL_Init(
        SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);  // Initialize SDL library

    if (sdl_status == -1) {
        std::string debug_msg =
            "SDL_Init: " + static_cast<std::string>(SDL_GetError());
        std::cerr << debug_msg << std::endl;
        return -1;
    }

    SDL_GameController *gamecontroller =
        SDL_GameControllerOpen(0);  // Open Game Controller

    // Create window
    SDL_Window *win =
        SDL_CreateWindow("2D Platformer", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, LEVEL_WIDTH, LEVEL_HEIGHT, 0);

    int open_audio_status =
        Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2,
                      chunksize);  // Initialize SDL mixer

    if (open_audio_status == -1) {
        std::string debug_msg =
            "Mix_OpenAudio: " + static_cast<std::string>(Mix_GetError());
        std::cerr << debug_msg << std::endl;
        return -1;
    }

    // Creates a renderer to render the images
    // * SDL_RENDERER_SOFTWARE starts the program using the CPU hardware
    // * SDL_RENDERER_ACCELERATED starts the program using the GPU hardware
    SDL_Renderer *rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(rend, 134, 191, 255, 255);

    /* Loads images, music, and soundeffects */
    // Creates the asset that loads the image into main memory
    SDL_Surface *player_surf = IMG_Load(player_path);

    if (player_surf == NULL) {
        std::string debug_msg =
            "IMG_Load: " + static_cast<std::string>(IMG_GetError());
        std::cerr << debug_msg << std::endl;
        return -1;
    }

    SDL_Surface *wall_surf = IMG_Load(wall_path);

    if (wall_surf == NULL) {
        std::string debug_msg =
            "IMG_Load: " + static_cast<std::string>(IMG_GetError());
        std::cerr << debug_msg << std::endl;
        return -1;
    }

    SDL_Surface *platform_surf = IMG_Load(platform_path);

    if (platform_surf == NULL) {
        std::string debug_msg =
            "IMG_Load: " + static_cast<std::string>(IMG_GetError());
        std::cerr << debug_msg << std::endl;
        return -1;
    }

    SDL_Surface *background_surf = IMG_Load(background_path);

    if (background_surf == NULL) {
        std::string debug_msg =
            "IMG_Load: " + static_cast<std::string>(IMG_GetError());
        std::cerr << debug_msg << std::endl;
        return -1;
    }

    Mix_Music *music = Mix_LoadMUS(music_path);

    if (music == NULL) {
        std::string debug_msg =
            "Mix_LoadMUS: " + static_cast<std::string>(Mix_GetError());
        std::cerr << debug_msg << std::endl;
        return -1;
    }

    // Loads images to our graphics hardware memory
    // Player structure
    SDL_Texture *player_tex = SDL_CreateTextureFromSurface(rend, player_surf);
    SDL_Rect p_dstrect = {0 + player_offset,
                          LEVEL_HEIGHT - player_height - player_offset,
                          player_width, player_height};
    SDL_Rect p_srcrect = {0, 0, player_width, player_height};

    MotionState motion_state;
    motion_state.jump = false;
    motion_state.jump_frames = 0;

    CollisionState collision_state;
    collision_state.on_the_floor = false;
    collision_state.on_the_platform = false;

    Player player;
    player.dstrect = p_dstrect;
    player.srcrect = p_srcrect;
    player.speed = player_speed;
    player.texture = player_tex;
    player.accel = player_accel;
    player.motion_state = motion_state;
    player.collision_state = collision_state;

    // Background structure
    SDL_Texture *background_tex =
        SDL_CreateTextureFromSurface(rend, background_surf);

    if (background_tex == NULL) {
        std::string debug_msg = "SDL_CreateTextureFromSurface: " +
                                static_cast<std::string>(SDL_GetError());
        std::cerr << debug_msg << std::endl;
        return -1;
    }

    SDL_Rect b_dstrect = {0, 0, background_width, background_height};

    SDL_Rect b_srcrect = {0, 0, background_source_width,
                          background_source_height};

    Background background;
    background.dstrect = b_dstrect;
    background.srcrect = b_srcrect;
    background.texture = background_tex;

    // Wall structure
    SDL_Texture *wall_tex = SDL_CreateTextureFromSurface(rend, wall_surf);

    if (wall_tex == NULL) {
        std::string debug_msg = "SDL_CreateTextureFromSurface: " +
                                static_cast<std::string>(SDL_GetError());
        std::cerr << debug_msg << std::endl;
        return -1;
    }

    SDL_Rect w_dstrect = {LEVEL_WIDTH - 200, LEVEL_HEIGHT - 200, wall_width,
                          wall_height};

    SDL_Rect w_srcrect = {0, 0, wall_source_width, wall_source_height};
    Wall wall;
    wall.dstrect = w_dstrect;
    wall.srcrect = w_srcrect;
    wall.texture = wall_tex;

    // Platform structure
    SDL_Texture *platform_tex =
        SDL_CreateTextureFromSurface(rend, platform_surf);

    if (platform_tex == NULL) {
        std::string debug_msg = "SDL_CreateTextureFromSurface: " +
                                static_cast<std::string>(SDL_GetError());
        std::cerr << debug_msg << std::endl;
        return -1;
    }

    SDL_Rect pl_dstrect = {LEVEL_WIDTH - 200, LEVEL_HEIGHT - 200,
                           platform_width, platform_height};

    SDL_Rect pl_srcrect = {0, 0, platform_source_width, platform_source_height};
    Platform platform;
    platform.dstrect = pl_dstrect;
    platform.srcrect = pl_srcrect;
    platform.texture = platform_tex;

    // Walls
    std::array<Wall, 45> walls = {
        wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall,
        wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall,
        wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall,
        wall, wall, wall, wall, wall, wall, wall, wall, wall};

    // Platforms
    std::array<Platform, 14> platforms = {
        platform, platform, platform, platform, platform, platform, platform,
        platform, platform, platform, platform, platform, platform, platform,
    };

    /* Map layout */
    // Set positions of the wall
    SetPosition(&walls[0].dstrect,
                Coord2D{LEVEL_WIDTH - 24, LEVEL_HEIGHT - 24});
    SetPosition(&walls[1].dstrect,
                Coord2D{LEVEL_WIDTH - 48, LEVEL_HEIGHT - 24});
    SetPosition(&walls[2].dstrect,
                Coord2D{LEVEL_WIDTH - 72, LEVEL_HEIGHT - 24});
    SetPosition(&walls[3].dstrect,
                Coord2D{LEVEL_WIDTH - 96, LEVEL_HEIGHT - 24});
    SetPosition(&walls[4].dstrect,
                Coord2D{LEVEL_WIDTH - 120, LEVEL_HEIGHT - 24});
    SetPosition(&walls[5].dstrect,
                Coord2D{LEVEL_WIDTH - 144, LEVEL_HEIGHT - 24});
    SetPosition(&walls[6].dstrect,
                Coord2D{LEVEL_WIDTH - 168, LEVEL_HEIGHT - 24});
    SetPosition(&walls[7].dstrect,
                Coord2D{LEVEL_WIDTH - 192, LEVEL_HEIGHT - 24});
    SetPosition(&walls[8].dstrect,
                Coord2D{LEVEL_WIDTH - 216, LEVEL_HEIGHT - 24});
    SetPosition(&walls[9].dstrect,
                Coord2D{LEVEL_WIDTH - 240, LEVEL_HEIGHT - 24});

    SetPosition(&walls[10].dstrect,
                Coord2D{LEVEL_WIDTH - 264, LEVEL_HEIGHT - 24});
    SetPosition(&walls[11].dstrect,
                Coord2D{LEVEL_WIDTH - 288, LEVEL_HEIGHT - 24});
    SetPosition(&walls[12].dstrect,
                Coord2D{LEVEL_WIDTH - 312, LEVEL_HEIGHT - 24});
    SetPosition(&walls[13].dstrect,
                Coord2D{LEVEL_WIDTH - 336, LEVEL_HEIGHT - 24});
    SetPosition(&walls[14].dstrect,
                Coord2D{LEVEL_WIDTH - 360, LEVEL_HEIGHT - 24});
    SetPosition(&walls[15].dstrect,
                Coord2D{LEVEL_WIDTH - 384, LEVEL_HEIGHT - 24});
    SetPosition(&walls[16].dstrect,
                Coord2D{LEVEL_WIDTH - 408, LEVEL_HEIGHT - 24});
    SetPosition(&walls[17].dstrect,
                Coord2D{LEVEL_WIDTH - 432, LEVEL_HEIGHT - 24});
    SetPosition(&walls[18].dstrect,
                Coord2D{LEVEL_WIDTH - 456, LEVEL_HEIGHT - 24});
    SetPosition(&walls[19].dstrect,
                Coord2D{LEVEL_WIDTH - 480, LEVEL_HEIGHT - 24});

    SetPosition(&walls[20].dstrect,
                Coord2D{LEVEL_WIDTH - 504, LEVEL_HEIGHT - 24});
    SetPosition(&walls[21].dstrect,
                Coord2D{LEVEL_WIDTH - 528, LEVEL_HEIGHT - 24});
    SetPosition(&walls[22].dstrect,
                Coord2D{LEVEL_WIDTH - 552, LEVEL_HEIGHT - 24});
    SetPosition(&walls[23].dstrect,
                Coord2D{LEVEL_WIDTH - 576, LEVEL_HEIGHT - 24});
    SetPosition(&walls[24].dstrect,
                Coord2D{LEVEL_WIDTH - 600, LEVEL_HEIGHT - 24});
    SetPosition(&walls[25].dstrect,
                Coord2D{LEVEL_WIDTH - 624, LEVEL_HEIGHT - 24});
    SetPosition(&walls[26].dstrect,
                Coord2D{LEVEL_WIDTH - 648, LEVEL_HEIGHT - 24});
    SetPosition(&walls[27].dstrect,
                Coord2D{LEVEL_WIDTH - 672, LEVEL_HEIGHT - 24});
    SetPosition(&walls[28].dstrect,
                Coord2D{LEVEL_WIDTH - 696, LEVEL_HEIGHT - 24});
    SetPosition(&walls[29].dstrect,
                Coord2D{LEVEL_WIDTH - 720, LEVEL_HEIGHT - 24});

    SetPosition(&walls[30].dstrect,
                Coord2D{LEVEL_WIDTH - 744, LEVEL_HEIGHT - 24});
    SetPosition(&walls[31].dstrect,
                Coord2D{LEVEL_WIDTH - 216, LEVEL_HEIGHT - 72});
    SetPosition(&walls[32].dstrect,
                Coord2D{LEVEL_WIDTH - 240, LEVEL_HEIGHT - 72});
    SetPosition(&walls[33].dstrect,
                Coord2D{LEVEL_WIDTH - 264, LEVEL_HEIGHT - 72});
    SetPosition(&walls[34].dstrect,
                Coord2D{LEVEL_WIDTH - 288, LEVEL_HEIGHT - 72});
    SetPosition(&walls[35].dstrect,
                Coord2D{LEVEL_WIDTH - 312, LEVEL_HEIGHT - 72});
    SetPosition(&walls[36].dstrect,
                Coord2D{LEVEL_WIDTH - 336, LEVEL_HEIGHT - 72});
    SetPosition(&walls[37].dstrect,
                Coord2D{LEVEL_WIDTH - 360, LEVEL_HEIGHT - 72});
    SetPosition(&walls[38].dstrect,
                Coord2D{LEVEL_WIDTH - 384, LEVEL_HEIGHT - 72});
    SetPosition(&walls[39].dstrect,
                Coord2D{LEVEL_WIDTH - 408, LEVEL_HEIGHT - 72});

    SetPosition(&walls[40].dstrect,
                Coord2D{LEVEL_WIDTH - 432, LEVEL_HEIGHT - 72});
    SetPosition(&walls[41].dstrect,
                Coord2D{LEVEL_WIDTH - 456, LEVEL_HEIGHT - 72});
    SetPosition(&walls[42].dstrect,
                Coord2D{LEVEL_WIDTH - 480, LEVEL_HEIGHT - 72});
    SetPosition(&walls[43].dstrect,
                Coord2D{LEVEL_WIDTH - 504, LEVEL_HEIGHT - 72});
    SetPosition(&walls[44].dstrect,
                Coord2D{LEVEL_WIDTH - 528, LEVEL_HEIGHT - 72});

    // Set the positions of the platforms
    SetPosition(&platforms[0].dstrect,
                Coord2D{LEVEL_WIDTH - 216, LEVEL_HEIGHT - 120});
    SetPosition(&platforms[1].dstrect,
                Coord2D{LEVEL_WIDTH - 240, LEVEL_HEIGHT - 120});
    SetPosition(&platforms[2].dstrect,
                Coord2D{LEVEL_WIDTH - 264, LEVEL_HEIGHT - 120});
    SetPosition(&platforms[3].dstrect,
                Coord2D{LEVEL_WIDTH - 288, LEVEL_HEIGHT - 120});
    SetPosition(&platforms[4].dstrect,
                Coord2D{LEVEL_WIDTH - 312, LEVEL_HEIGHT - 120});
    SetPosition(&platforms[5].dstrect,
                Coord2D{LEVEL_WIDTH - 336, LEVEL_HEIGHT - 120});
    SetPosition(&platforms[6].dstrect,
                Coord2D{LEVEL_WIDTH - 360, LEVEL_HEIGHT - 120});
    SetPosition(&platforms[7].dstrect,
                Coord2D{LEVEL_WIDTH - 384, LEVEL_HEIGHT - 120});
    SetPosition(&platforms[8].dstrect,
                Coord2D{LEVEL_WIDTH - 408, LEVEL_HEIGHT - 120});
    SetPosition(&platforms[9].dstrect,
                Coord2D{LEVEL_WIDTH - 432, LEVEL_HEIGHT - 120});
    SetPosition(&platforms[10].dstrect,
                Coord2D{LEVEL_WIDTH - 456, LEVEL_HEIGHT - 120});
    SetPosition(&platforms[11].dstrect,
                Coord2D{LEVEL_WIDTH - 480, LEVEL_HEIGHT - 120});
    SetPosition(&platforms[12].dstrect,
                Coord2D{LEVEL_WIDTH - 504, LEVEL_HEIGHT - 120});
    SetPosition(&platforms[13].dstrect,
                Coord2D{LEVEL_WIDTH - 528, LEVEL_HEIGHT - 120});

    Mix_VolumeMusic(music_volume);  // Adjust music volume

    int player_music_status =
        Mix_PlayMusic(music, -1);  // Start background music (-1 means infinity)

    if (player_music_status == -1) {
        std::string debug_msg =
            "Mix_PlayMusic: " + static_cast<std::string>(Mix_GetError());
        std::cerr << debug_msg << std::endl;
        return -1;
    }

    /* Gameplay Loop */
    bool quit = false;  // gameplay loop switch

    while (!quit) {  // gameplay loop
        /* Click Key Bindings */
        SDL_Event event;  // Event handling

        while (SDL_PollEvent(&event) == 1) {  // Events management
            // Click Keybindings
            quit = ClickKeybindings(event, &player.motion_state,
                                    &player.collision_state, &player.dstrect,
                                    player.accel);
        }

        /* Hold Keybindings */
        HoldKeybindings(&player, gamecontroller);

        /* Player boundaries */
        PlayerBoundary(&player);

        /* Render sprites */
        RenderSprites(rend, player, walls, platforms, background);

        /* Gravity */
        Gravity(&player);

        /* Jump physics */
        JumpPhysics(&player, &player.motion_state);

        /* Player wall collisons */
        PlayerObjectCollisions(&player, walls, platforms,
                               &player.collision_state);
    }

    /* Free resources and close SDL and SDL mixer */
    FreeAndCloseResources(player_surf, player_tex, wall_surf, wall_tex, music,
                          rend, win, gamecontroller);

    return 0;
}

void PlayerBoundary(Player *player) {
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

void RenderSprites(SDL_Renderer *rend, Player player,
                   std::array<Wall, 45> walls,
                   std::array<Platform, 14> platforms, Background background) {
    /* Frames per second */
    const int miliseconds = 1000;    // 1000 ms equals 1s
    const int gameplay_frames = 60;  // amount of frames per second

    /* Render sprites */
    SDL_RenderClear(rend);

    // Render background
    SDL_RenderCopy(rend, background.texture, &background.srcrect,
                   &background.dstrect);

    // Render Walls
    SDL_RenderCopy(rend, walls[0].texture, &walls[0].srcrect,
                   &walls[0].dstrect);
    SDL_RenderCopy(rend, walls[1].texture, &walls[1].srcrect,
                   &walls[1].dstrect);
    SDL_RenderCopy(rend, walls[2].texture, &walls[2].srcrect,
                   &walls[2].dstrect);
    SDL_RenderCopy(rend, walls[3].texture, &walls[3].srcrect,
                   &walls[3].dstrect);
    SDL_RenderCopy(rend, walls[4].texture, &walls[4].srcrect,
                   &walls[4].dstrect);
    SDL_RenderCopy(rend, walls[5].texture, &walls[5].srcrect,
                   &walls[5].dstrect);
    SDL_RenderCopy(rend, walls[6].texture, &walls[6].srcrect,
                   &walls[6].dstrect);
    SDL_RenderCopy(rend, walls[7].texture, &walls[7].srcrect,
                   &walls[7].dstrect);
    SDL_RenderCopy(rend, walls[8].texture, &walls[8].srcrect,
                   &walls[8].dstrect);
    SDL_RenderCopy(rend, walls[9].texture, &walls[9].srcrect,
                   &walls[9].dstrect);

    SDL_RenderCopy(rend, walls[10].texture, &walls[10].srcrect,
                   &walls[10].dstrect);
    SDL_RenderCopy(rend, walls[11].texture, &walls[11].srcrect,
                   &walls[11].dstrect);
    SDL_RenderCopy(rend, walls[12].texture, &walls[12].srcrect,
                   &walls[12].dstrect);
    SDL_RenderCopy(rend, walls[13].texture, &walls[13].srcrect,
                   &walls[13].dstrect);
    SDL_RenderCopy(rend, walls[14].texture, &walls[14].srcrect,
                   &walls[14].dstrect);
    SDL_RenderCopy(rend, walls[15].texture, &walls[15].srcrect,
                   &walls[15].dstrect);
    SDL_RenderCopy(rend, walls[16].texture, &walls[16].srcrect,
                   &walls[16].dstrect);
    SDL_RenderCopy(rend, walls[17].texture, &walls[17].srcrect,
                   &walls[17].dstrect);
    SDL_RenderCopy(rend, walls[18].texture, &walls[18].srcrect,
                   &walls[18].dstrect);
    SDL_RenderCopy(rend, walls[19].texture, &walls[19].srcrect,
                   &walls[19].dstrect);

    SDL_RenderCopy(rend, walls[20].texture, &walls[20].srcrect,
                   &walls[20].dstrect);
    SDL_RenderCopy(rend, walls[21].texture, &walls[21].srcrect,
                   &walls[21].dstrect);
    SDL_RenderCopy(rend, walls[22].texture, &walls[22].srcrect,
                   &walls[22].dstrect);
    SDL_RenderCopy(rend, walls[23].texture, &walls[23].srcrect,
                   &walls[23].dstrect);
    SDL_RenderCopy(rend, walls[24].texture, &walls[24].srcrect,
                   &walls[24].dstrect);
    SDL_RenderCopy(rend, walls[25].texture, &walls[25].srcrect,
                   &walls[25].dstrect);
    SDL_RenderCopy(rend, walls[26].texture, &walls[26].srcrect,
                   &walls[26].dstrect);
    SDL_RenderCopy(rend, walls[27].texture, &walls[27].srcrect,
                   &walls[27].dstrect);
    SDL_RenderCopy(rend, walls[28].texture, &walls[28].srcrect,
                   &walls[28].dstrect);
    SDL_RenderCopy(rend, walls[29].texture, &walls[29].srcrect,
                   &walls[29].dstrect);

    SDL_RenderCopy(rend, walls[30].texture, &walls[30].srcrect,
                   &walls[30].dstrect);
    SDL_RenderCopy(rend, walls[31].texture, &walls[31].srcrect,
                   &walls[31].dstrect);
    SDL_RenderCopy(rend, walls[32].texture, &walls[32].srcrect,
                   &walls[32].dstrect);
    SDL_RenderCopy(rend, walls[33].texture, &walls[33].srcrect,
                   &walls[33].dstrect);
    SDL_RenderCopy(rend, walls[34].texture, &walls[34].srcrect,
                   &walls[34].dstrect);
    SDL_RenderCopy(rend, walls[35].texture, &walls[35].srcrect,
                   &walls[35].dstrect);
    SDL_RenderCopy(rend, walls[36].texture, &walls[36].srcrect,
                   &walls[36].dstrect);
    SDL_RenderCopy(rend, walls[37].texture, &walls[37].srcrect,
                   &walls[37].dstrect);
    SDL_RenderCopy(rend, walls[38].texture, &walls[38].srcrect,
                   &walls[38].dstrect);
    SDL_RenderCopy(rend, walls[39].texture, &walls[39].srcrect,
                   &walls[39].dstrect);
    SDL_RenderCopy(rend, walls[40].texture, &walls[40].srcrect,
                   &walls[40].dstrect);
    SDL_RenderCopy(rend, walls[41].texture, &walls[41].srcrect,
                   &walls[41].dstrect);
    SDL_RenderCopy(rend, walls[42].texture, &walls[42].srcrect,
                   &walls[42].dstrect);
    SDL_RenderCopy(rend, walls[43].texture, &walls[43].srcrect,
                   &walls[43].dstrect);
    SDL_RenderCopy(rend, walls[44].texture, &walls[44].srcrect,
                   &walls[44].dstrect);

    // Render platforms
    SDL_RenderCopy(rend, platforms[0].texture, &platforms[0].srcrect,
                   &platforms[0].dstrect);
    SDL_RenderCopy(rend, platforms[1].texture, &platforms[1].srcrect,
                   &platforms[1].dstrect);
    SDL_RenderCopy(rend, platforms[2].texture, &platforms[2].srcrect,
                   &platforms[2].dstrect);
    SDL_RenderCopy(rend, platforms[3].texture, &platforms[3].srcrect,
                   &platforms[3].dstrect);
    SDL_RenderCopy(rend, platforms[4].texture, &platforms[4].srcrect,
                   &platforms[4].dstrect);
    SDL_RenderCopy(rend, platforms[5].texture, &platforms[5].srcrect,
                   &platforms[5].dstrect);
    SDL_RenderCopy(rend, platforms[6].texture, &platforms[6].srcrect,
                   &platforms[6].dstrect);
    SDL_RenderCopy(rend, platforms[7].texture, &platforms[7].srcrect,
                   &platforms[7].dstrect);
    SDL_RenderCopy(rend, platforms[8].texture, &platforms[8].srcrect,
                   &platforms[8].dstrect);
    SDL_RenderCopy(rend, platforms[9].texture, &platforms[9].srcrect,
                   &platforms[9].dstrect);
    SDL_RenderCopy(rend, platforms[10].texture, &platforms[10].srcrect,
                   &platforms[10].dstrect);
    SDL_RenderCopy(rend, platforms[11].texture, &platforms[11].srcrect,
                   &platforms[11].dstrect);
    SDL_RenderCopy(rend, platforms[12].texture, &platforms[12].srcrect,
                   &platforms[12].dstrect);
    SDL_RenderCopy(rend, platforms[13].texture, &platforms[13].srcrect,
                   &platforms[13].dstrect);

    SDL_RenderCopy(rend, player.texture, &player.srcrect, &player.dstrect);
    SDL_RenderPresent(rend);  // Triggers double buffers for multiple rendering
    SDL_Delay(miliseconds / gameplay_frames);  // Calculates to 60 fps
}

void SetPosition(SDL_Rect *dstrect, Coord2D pos) {
    dstrect->x = pos.x;
    dstrect->y = pos.y;
}

void PlayerObjectCollisions(Player *player, std::array<Wall, 45> walls,
                            std::array<Platform, 14> platforms,
                            CollisionState *collision_state) {
    /* Player wall collisons */
    PlayerWallCollision(player, &walls[0], collision_state);
    PlayerWallCollision(player, &walls[1], collision_state);
    PlayerWallCollision(player, &walls[2], collision_state);
    PlayerWallCollision(player, &walls[3], collision_state);
    PlayerWallCollision(player, &walls[4], collision_state);
    PlayerWallCollision(player, &walls[5], collision_state);
    PlayerWallCollision(player, &walls[6], collision_state);
    PlayerWallCollision(player, &walls[7], collision_state);
    PlayerWallCollision(player, &walls[8], collision_state);
    PlayerWallCollision(player, &walls[9], collision_state);

    PlayerWallCollision(player, &walls[10], collision_state);
    PlayerWallCollision(player, &walls[11], collision_state);
    PlayerWallCollision(player, &walls[12], collision_state);
    PlayerWallCollision(player, &walls[13], collision_state);
    PlayerWallCollision(player, &walls[14], collision_state);
    PlayerWallCollision(player, &walls[15], collision_state);
    PlayerWallCollision(player, &walls[16], collision_state);
    PlayerWallCollision(player, &walls[17], collision_state);
    PlayerWallCollision(player, &walls[18], collision_state);
    PlayerWallCollision(player, &walls[19], collision_state);

    PlayerWallCollision(player, &walls[20], collision_state);
    PlayerWallCollision(player, &walls[21], collision_state);
    PlayerWallCollision(player, &walls[22], collision_state);
    PlayerWallCollision(player, &walls[23], collision_state);
    PlayerWallCollision(player, &walls[24], collision_state);
    PlayerWallCollision(player, &walls[25], collision_state);
    PlayerWallCollision(player, &walls[26], collision_state);
    PlayerWallCollision(player, &walls[27], collision_state);
    PlayerWallCollision(player, &walls[28], collision_state);
    PlayerWallCollision(player, &walls[29], collision_state);

    PlayerWallCollision(player, &walls[30], collision_state);
    PlayerWallCollision(player, &walls[31], collision_state);
    PlayerWallCollision(player, &walls[32], collision_state);
    PlayerWallCollision(player, &walls[33], collision_state);
    PlayerWallCollision(player, &walls[34], collision_state);
    PlayerWallCollision(player, &walls[35], collision_state);
    PlayerWallCollision(player, &walls[36], collision_state);
    PlayerWallCollision(player, &walls[37], collision_state);
    PlayerWallCollision(player, &walls[38], collision_state);
    PlayerWallCollision(player, &walls[39], collision_state);

    PlayerWallCollision(player, &walls[40], collision_state);
    PlayerWallCollision(player, &walls[41], collision_state);
    PlayerWallCollision(player, &walls[42], collision_state);
    PlayerWallCollision(player, &walls[43], collision_state);
    PlayerWallCollision(player, &walls[44], collision_state);

    /* Player PLatform Collisions */
    PlayerPlatformCollision(player, &platforms[0], collision_state);
    PlayerPlatformCollision(player, &platforms[1], collision_state);
    PlayerPlatformCollision(player, &platforms[2], collision_state);
    PlayerPlatformCollision(player, &platforms[3], collision_state);
    PlayerPlatformCollision(player, &platforms[4], collision_state);
    PlayerPlatformCollision(player, &platforms[5], collision_state);
    PlayerPlatformCollision(player, &platforms[6], collision_state);
    PlayerPlatformCollision(player, &platforms[7], collision_state);
    PlayerPlatformCollision(player, &platforms[8], collision_state);
    PlayerPlatformCollision(player, &platforms[9], collision_state);
    PlayerPlatformCollision(player, &platforms[10], collision_state);
    PlayerPlatformCollision(player, &platforms[11], collision_state);
    PlayerPlatformCollision(player, &platforms[12], collision_state);
    PlayerPlatformCollision(player, &platforms[13], collision_state);
}

void FreeAndCloseResources(SDL_Surface *player_surf, SDL_Texture *player_tex,
                           SDL_Surface *wall_surf, SDL_Texture *wall_tex,
                           Mix_Music *music, SDL_Renderer *rend,
                           SDL_Window *win,
                           SDL_GameController *gamecontroller) {
    /* Free resources and close SDL and SDL mixer */
    Mix_FreeMusic(music);  // Free the music

    // Deallocate textues and surfaces
    SDL_FreeSurface(player_surf);    // Deallocate player and scene surfaces
    SDL_DestroyTexture(player_tex);  // Destroy scene and player textures
    SDL_FreeSurface(wall_surf);      // Deallocate player and scene surfaces
    SDL_DestroyTexture(wall_tex);    // Destroy scene and player textures

    // Close Game Controller
    SDL_GameControllerClose(gamecontroller);

    SDL_DestroyRenderer(rend);  // Destroy renderer
    SDL_DestroyWindow(win);     // Destroy window
    Mix_CloseAudio();           // Close Audio
    IMG_Quit();                 // Close Image
    SDL_Quit();                 // Quit SDL subsystems
}
