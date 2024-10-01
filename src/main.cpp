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
                   std::array<Block, 52> blocks,
                   std::array<Platform, 6> platforms, Background background);

void SetPosition(SDL_Rect *dstrect, Coord2D pos);

void FreeAndCloseResources(SDL_Surface *player_surf, SDL_Texture *player_tex,
                           SDL_Surface *block_surf, SDL_Texture *block_tex,
                           Mix_Music *music, SDL_Renderer *rend,
                           SDL_Window *win, SDL_GameController *gamecontroller);

void PlayerObjectCollisions(Player *player, std::array<Block, 52> blocks,
                            std::array<Platform, 6> platforms,
                            CollisionState *collision_state);

int main() {
    // Player Attributes
    const int player_width = 24;
    const int player_height = 24;
    const int player_speed = 2;    // speed of player
    const int player_offset = 24;  // gap between left corner of the window
    const int player_accel = 4;

    // Block dimensions
    const int block_width = 24;
    const int block_height = 24;

    const int block_source_width = 512;
    const int block_source_height = 512;

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
    const char *block_path = "assets/tiles/block.png";
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

    SDL_Surface *block_surf = IMG_Load(block_path);

    if (block_surf == NULL) {
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

    // Block structure
    SDL_Texture *block_tex = SDL_CreateTextureFromSurface(rend, block_surf);

    if (block_tex == NULL) {
        std::string debug_msg = "SDL_CreateTextureFromSurface: " +
                                static_cast<std::string>(SDL_GetError());
        std::cerr << debug_msg << std::endl;
        return -1;
    }

    SDL_Rect w_dstrect = {LEVEL_WIDTH - 200, LEVEL_HEIGHT - 200, block_width,
                          block_height};

    SDL_Rect w_srcrect = {0, 0, block_source_width, block_source_height};
    Block block;
    block.dstrect = w_dstrect;
    block.srcrect = w_srcrect;
    block.texture = block_tex;

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

    // Blocks
    std::array<Block, 52> blocks = {
        block, block, block, block, block, block, block, block, block,
        block, block, block, block, block, block, block, block, block,
        block, block, block, block, block, block, block, block, block,
        block, block, block, block, block, block, block, block, block,
        block, block, block, block, block, block, block, block, block,
        block, block, block, block, block, block, block,
    };

    // Platforms
    std::array<Platform, 6> platforms = {
        platform, platform, platform, platform, platform, platform,
    };

    /* Map layout */
    // Set positions of the block
    SetPosition(&blocks[0].dstrect,
                Coord2D{LEVEL_WIDTH - 24, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[1].dstrect,
                Coord2D{LEVEL_WIDTH - 48, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[2].dstrect,
                Coord2D{LEVEL_WIDTH - 72, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[3].dstrect,
                Coord2D{LEVEL_WIDTH - 96, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[4].dstrect,
                Coord2D{LEVEL_WIDTH - 120, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[5].dstrect,
                Coord2D{LEVEL_WIDTH - 144, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[6].dstrect,
                Coord2D{LEVEL_WIDTH - 168, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[7].dstrect,
                Coord2D{LEVEL_WIDTH - 192, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[8].dstrect,
                Coord2D{LEVEL_WIDTH - 216, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[9].dstrect,
                Coord2D{LEVEL_WIDTH - 240, LEVEL_HEIGHT - 24});

    SetPosition(&blocks[10].dstrect,
                Coord2D{LEVEL_WIDTH - 264, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[11].dstrect,
                Coord2D{LEVEL_WIDTH - 288, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[12].dstrect,
                Coord2D{LEVEL_WIDTH - 312, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[13].dstrect,
                Coord2D{LEVEL_WIDTH - 336, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[14].dstrect,
                Coord2D{LEVEL_WIDTH - 360, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[15].dstrect,
                Coord2D{LEVEL_WIDTH - 384, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[16].dstrect,
                Coord2D{LEVEL_WIDTH - 408, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[17].dstrect,
                Coord2D{LEVEL_WIDTH - 432, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[18].dstrect,
                Coord2D{LEVEL_WIDTH - 456, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[19].dstrect,
                Coord2D{LEVEL_WIDTH - 480, LEVEL_HEIGHT - 24});

    SetPosition(&blocks[20].dstrect,
                Coord2D{LEVEL_WIDTH - 504, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[21].dstrect,
                Coord2D{LEVEL_WIDTH - 528, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[22].dstrect,
                Coord2D{LEVEL_WIDTH - 552, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[23].dstrect,
                Coord2D{LEVEL_WIDTH - 576, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[24].dstrect,
                Coord2D{LEVEL_WIDTH - 600, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[25].dstrect,
                Coord2D{LEVEL_WIDTH - 624, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[26].dstrect,
                Coord2D{LEVEL_WIDTH - 648, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[27].dstrect,
                Coord2D{LEVEL_WIDTH - 672, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[28].dstrect,
                Coord2D{LEVEL_WIDTH - 696, LEVEL_HEIGHT - 24});
    SetPosition(&blocks[29].dstrect,
                Coord2D{LEVEL_WIDTH - 720, LEVEL_HEIGHT - 24});

    SetPosition(&blocks[30].dstrect,
                Coord2D{LEVEL_WIDTH - 744, LEVEL_HEIGHT - 24});

    SetPosition(&blocks[31].dstrect,
                Coord2D{LEVEL_WIDTH - 360, LEVEL_HEIGHT - 216});
    SetPosition(&blocks[32].dstrect,
                Coord2D{LEVEL_WIDTH - 384, LEVEL_HEIGHT - 216});
    SetPosition(&blocks[33].dstrect,
                Coord2D{LEVEL_WIDTH - 384, LEVEL_HEIGHT - 192});
    SetPosition(&blocks[34].dstrect,
                Coord2D{LEVEL_WIDTH - 408, LEVEL_HEIGHT - 192});
    SetPosition(&blocks[35].dstrect,
                Coord2D{LEVEL_WIDTH - 408, LEVEL_HEIGHT - 168});
    SetPosition(&blocks[36].dstrect,
                Coord2D{LEVEL_WIDTH - 432, LEVEL_HEIGHT - 168});
    SetPosition(&blocks[37].dstrect,
                Coord2D{LEVEL_WIDTH - 432, LEVEL_HEIGHT - 144});
    SetPosition(&blocks[38].dstrect,
                Coord2D{LEVEL_WIDTH - 456, LEVEL_HEIGHT - 144});
    SetPosition(&blocks[39].dstrect,
                Coord2D{LEVEL_WIDTH - 456, LEVEL_HEIGHT - 120});
    SetPosition(&blocks[40].dstrect,
                Coord2D{LEVEL_WIDTH - 480, LEVEL_HEIGHT - 120});
    SetPosition(&blocks[41].dstrect,
                Coord2D{LEVEL_WIDTH - 480, LEVEL_HEIGHT - 96});
    SetPosition(&blocks[42].dstrect,
                Coord2D{LEVEL_WIDTH - 504, LEVEL_HEIGHT - 96});
    SetPosition(&blocks[43].dstrect,
                Coord2D{LEVEL_WIDTH - 504, LEVEL_HEIGHT - 72});
    SetPosition(&blocks[44].dstrect,
                Coord2D{LEVEL_WIDTH - 528, LEVEL_HEIGHT - 72});

    // Set the positions of the platforms
    SetPosition(&platforms[0].dstrect,
                Coord2D{LEVEL_WIDTH - 408, LEVEL_HEIGHT - 240});
    SetPosition(&platforms[1].dstrect,
                Coord2D{LEVEL_WIDTH - 432, LEVEL_HEIGHT - 264});
    SetPosition(&platforms[2].dstrect,
                Coord2D{LEVEL_WIDTH - 456, LEVEL_HEIGHT - 288});
    SetPosition(&platforms[3].dstrect,
                Coord2D{LEVEL_WIDTH - 480, LEVEL_HEIGHT - 312});
    SetPosition(&platforms[4].dstrect,
                Coord2D{LEVEL_WIDTH - 504, LEVEL_HEIGHT - 312});
    SetPosition(&platforms[5].dstrect,
                Coord2D{LEVEL_WIDTH - 528, LEVEL_HEIGHT - 312});

    // Set the positions of the blocks
    SetPosition(&blocks[45].dstrect,
                Coord2D{LEVEL_WIDTH - 432, LEVEL_HEIGHT - 360});
    SetPosition(&blocks[46].dstrect,
                Coord2D{LEVEL_WIDTH - 384, LEVEL_HEIGHT - 360});
    SetPosition(&blocks[47].dstrect,
                Coord2D{LEVEL_WIDTH - 336, LEVEL_HEIGHT - 360});
    SetPosition(&blocks[48].dstrect,
                Coord2D{LEVEL_WIDTH - 288, LEVEL_HEIGHT - 360});
    SetPosition(&blocks[49].dstrect,
                Coord2D{LEVEL_WIDTH - 240, LEVEL_HEIGHT - 360});
    SetPosition(&blocks[50].dstrect,
                Coord2D{LEVEL_WIDTH - 192, LEVEL_HEIGHT - 360});
    SetPosition(&blocks[51].dstrect,
                Coord2D{LEVEL_WIDTH - 96, LEVEL_HEIGHT - 360});

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
        RenderSprites(rend, player, blocks, platforms, background);

        /* Gravity */
        Gravity(&player);

        /* Jump physics */
        JumpPhysics(&player, &player.motion_state);

        /* Player block collisons */
        PlayerObjectCollisions(&player, blocks, platforms,
                               &player.collision_state);
    }

    /* Free resources and close SDL and SDL mixer */
    FreeAndCloseResources(player_surf, player_tex, block_surf, block_tex, music,
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
                   std::array<Block, 52> blocks,
                   std::array<Platform, 6> platforms, Background background) {
    /* Frames per second */
    const int miliseconds = 1000;    // 1000 ms equals 1s
    const int gameplay_frames = 60;  // amount of frames per second

    /* Render sprites */
    SDL_RenderClear(rend);

    // Render background
    SDL_RenderCopy(rend, background.texture, &background.srcrect,
                   &background.dstrect);

    // Render Blocks
    SDL_RenderCopy(rend, blocks[0].texture, &blocks[0].srcrect,
                   &blocks[0].dstrect);
    SDL_RenderCopy(rend, blocks[1].texture, &blocks[1].srcrect,
                   &blocks[1].dstrect);
    SDL_RenderCopy(rend, blocks[2].texture, &blocks[2].srcrect,
                   &blocks[2].dstrect);
    SDL_RenderCopy(rend, blocks[3].texture, &blocks[3].srcrect,
                   &blocks[3].dstrect);
    SDL_RenderCopy(rend, blocks[4].texture, &blocks[4].srcrect,
                   &blocks[4].dstrect);
    SDL_RenderCopy(rend, blocks[5].texture, &blocks[5].srcrect,
                   &blocks[5].dstrect);
    SDL_RenderCopy(rend, blocks[6].texture, &blocks[6].srcrect,
                   &blocks[6].dstrect);
    SDL_RenderCopy(rend, blocks[7].texture, &blocks[7].srcrect,
                   &blocks[7].dstrect);
    SDL_RenderCopy(rend, blocks[8].texture, &blocks[8].srcrect,
                   &blocks[8].dstrect);
    SDL_RenderCopy(rend, blocks[9].texture, &blocks[9].srcrect,
                   &blocks[9].dstrect);

    SDL_RenderCopy(rend, blocks[10].texture, &blocks[10].srcrect,
                   &blocks[10].dstrect);
    SDL_RenderCopy(rend, blocks[11].texture, &blocks[11].srcrect,
                   &blocks[11].dstrect);
    SDL_RenderCopy(rend, blocks[12].texture, &blocks[12].srcrect,
                   &blocks[12].dstrect);
    SDL_RenderCopy(rend, blocks[13].texture, &blocks[13].srcrect,
                   &blocks[13].dstrect);
    SDL_RenderCopy(rend, blocks[14].texture, &blocks[14].srcrect,
                   &blocks[14].dstrect);
    SDL_RenderCopy(rend, blocks[15].texture, &blocks[15].srcrect,
                   &blocks[15].dstrect);
    SDL_RenderCopy(rend, blocks[16].texture, &blocks[16].srcrect,
                   &blocks[16].dstrect);
    SDL_RenderCopy(rend, blocks[17].texture, &blocks[17].srcrect,
                   &blocks[17].dstrect);
    SDL_RenderCopy(rend, blocks[18].texture, &blocks[18].srcrect,
                   &blocks[18].dstrect);
    SDL_RenderCopy(rend, blocks[19].texture, &blocks[19].srcrect,
                   &blocks[19].dstrect);

    SDL_RenderCopy(rend, blocks[20].texture, &blocks[20].srcrect,
                   &blocks[20].dstrect);
    SDL_RenderCopy(rend, blocks[21].texture, &blocks[21].srcrect,
                   &blocks[21].dstrect);
    SDL_RenderCopy(rend, blocks[22].texture, &blocks[22].srcrect,
                   &blocks[22].dstrect);
    SDL_RenderCopy(rend, blocks[23].texture, &blocks[23].srcrect,
                   &blocks[23].dstrect);
    SDL_RenderCopy(rend, blocks[24].texture, &blocks[24].srcrect,
                   &blocks[24].dstrect);
    SDL_RenderCopy(rend, blocks[25].texture, &blocks[25].srcrect,
                   &blocks[25].dstrect);
    SDL_RenderCopy(rend, blocks[26].texture, &blocks[26].srcrect,
                   &blocks[26].dstrect);
    SDL_RenderCopy(rend, blocks[27].texture, &blocks[27].srcrect,
                   &blocks[27].dstrect);
    SDL_RenderCopy(rend, blocks[28].texture, &blocks[28].srcrect,
                   &blocks[28].dstrect);
    SDL_RenderCopy(rend, blocks[29].texture, &blocks[29].srcrect,
                   &blocks[29].dstrect);

    SDL_RenderCopy(rend, blocks[30].texture, &blocks[30].srcrect,
                   &blocks[30].dstrect);
    SDL_RenderCopy(rend, blocks[31].texture, &blocks[31].srcrect,
                   &blocks[31].dstrect);
    SDL_RenderCopy(rend, blocks[32].texture, &blocks[32].srcrect,
                   &blocks[32].dstrect);
    SDL_RenderCopy(rend, blocks[33].texture, &blocks[33].srcrect,
                   &blocks[33].dstrect);
    SDL_RenderCopy(rend, blocks[34].texture, &blocks[34].srcrect,
                   &blocks[34].dstrect);
    SDL_RenderCopy(rend, blocks[35].texture, &blocks[35].srcrect,
                   &blocks[35].dstrect);
    SDL_RenderCopy(rend, blocks[36].texture, &blocks[36].srcrect,
                   &blocks[36].dstrect);
    SDL_RenderCopy(rend, blocks[37].texture, &blocks[37].srcrect,
                   &blocks[37].dstrect);
    SDL_RenderCopy(rend, blocks[38].texture, &blocks[38].srcrect,
                   &blocks[38].dstrect);
    SDL_RenderCopy(rend, blocks[39].texture, &blocks[39].srcrect,
                   &blocks[39].dstrect);
    SDL_RenderCopy(rend, blocks[40].texture, &blocks[40].srcrect,
                   &blocks[40].dstrect);
    SDL_RenderCopy(rend, blocks[41].texture, &blocks[41].srcrect,
                   &blocks[41].dstrect);
    SDL_RenderCopy(rend, blocks[42].texture, &blocks[42].srcrect,
                   &blocks[42].dstrect);
    SDL_RenderCopy(rend, blocks[43].texture, &blocks[43].srcrect,
                   &blocks[43].dstrect);
    SDL_RenderCopy(rend, blocks[44].texture, &blocks[44].srcrect,
                   &blocks[44].dstrect);
    SDL_RenderCopy(rend, blocks[45].texture, &blocks[45].srcrect,
                   &blocks[45].dstrect);
    SDL_RenderCopy(rend, blocks[46].texture, &blocks[46].srcrect,
                   &blocks[46].dstrect);
    SDL_RenderCopy(rend, blocks[47].texture, &blocks[47].srcrect,
                   &blocks[47].dstrect);
    SDL_RenderCopy(rend, blocks[48].texture, &blocks[48].srcrect,
                   &blocks[48].dstrect);
    SDL_RenderCopy(rend, blocks[49].texture, &blocks[49].srcrect,
                   &blocks[49].dstrect);
    SDL_RenderCopy(rend, blocks[50].texture, &blocks[50].srcrect,
                   &blocks[50].dstrect);
    SDL_RenderCopy(rend, blocks[51].texture, &blocks[51].srcrect,
                   &blocks[51].dstrect);

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

    SDL_RenderCopy(rend, player.texture, &player.srcrect, &player.dstrect);
    SDL_RenderPresent(rend);  // Triggers double buffers for multiple rendering
    SDL_Delay(miliseconds / gameplay_frames);  // Calculates to 60 fps
}

void SetPosition(SDL_Rect *dstrect, Coord2D pos) {
    dstrect->x = pos.x;
    dstrect->y = pos.y;
}

void PlayerObjectCollisions(Player *player, std::array<Block, 52> blocks,
                            std::array<Platform, 6> platforms,
                            CollisionState *collision_state) {
    /* Player block collisons */
    PlayerBlockCollision(player, &blocks[0], collision_state);
    PlayerBlockCollision(player, &blocks[1], collision_state);
    PlayerBlockCollision(player, &blocks[2], collision_state);
    PlayerBlockCollision(player, &blocks[3], collision_state);
    PlayerBlockCollision(player, &blocks[4], collision_state);
    PlayerBlockCollision(player, &blocks[5], collision_state);
    PlayerBlockCollision(player, &blocks[6], collision_state);
    PlayerBlockCollision(player, &blocks[7], collision_state);
    PlayerBlockCollision(player, &blocks[8], collision_state);
    PlayerBlockCollision(player, &blocks[9], collision_state);

    PlayerBlockCollision(player, &blocks[10], collision_state);
    PlayerBlockCollision(player, &blocks[11], collision_state);
    PlayerBlockCollision(player, &blocks[12], collision_state);
    PlayerBlockCollision(player, &blocks[13], collision_state);
    PlayerBlockCollision(player, &blocks[14], collision_state);
    PlayerBlockCollision(player, &blocks[15], collision_state);
    PlayerBlockCollision(player, &blocks[16], collision_state);
    PlayerBlockCollision(player, &blocks[17], collision_state);
    PlayerBlockCollision(player, &blocks[18], collision_state);
    PlayerBlockCollision(player, &blocks[19], collision_state);

    PlayerBlockCollision(player, &blocks[20], collision_state);
    PlayerBlockCollision(player, &blocks[21], collision_state);
    PlayerBlockCollision(player, &blocks[22], collision_state);
    PlayerBlockCollision(player, &blocks[23], collision_state);
    PlayerBlockCollision(player, &blocks[24], collision_state);
    PlayerBlockCollision(player, &blocks[25], collision_state);
    PlayerBlockCollision(player, &blocks[26], collision_state);
    PlayerBlockCollision(player, &blocks[27], collision_state);
    PlayerBlockCollision(player, &blocks[28], collision_state);
    PlayerBlockCollision(player, &blocks[29], collision_state);

    PlayerBlockCollision(player, &blocks[30], collision_state);
    PlayerBlockCollision(player, &blocks[31], collision_state);
    PlayerBlockCollision(player, &blocks[32], collision_state);
    PlayerBlockCollision(player, &blocks[33], collision_state);
    PlayerBlockCollision(player, &blocks[34], collision_state);
    PlayerBlockCollision(player, &blocks[35], collision_state);
    PlayerBlockCollision(player, &blocks[36], collision_state);
    PlayerBlockCollision(player, &blocks[37], collision_state);
    PlayerBlockCollision(player, &blocks[38], collision_state);
    PlayerBlockCollision(player, &blocks[39], collision_state);

    PlayerBlockCollision(player, &blocks[40], collision_state);
    PlayerBlockCollision(player, &blocks[41], collision_state);
    PlayerBlockCollision(player, &blocks[42], collision_state);
    PlayerBlockCollision(player, &blocks[43], collision_state);
    PlayerBlockCollision(player, &blocks[44], collision_state);
    PlayerBlockCollision(player, &blocks[45], collision_state);
    PlayerBlockCollision(player, &blocks[46], collision_state);
    PlayerBlockCollision(player, &blocks[47], collision_state);
    PlayerBlockCollision(player, &blocks[48], collision_state);
    PlayerBlockCollision(player, &blocks[49], collision_state);
    PlayerBlockCollision(player, &blocks[50], collision_state);

    PlayerBlockCollision(player, &blocks[51], collision_state);

    /* Player PLatform Collisions */
    PlayerPlatformCollision(player, &platforms[0], collision_state);
    PlayerPlatformCollision(player, &platforms[1], collision_state);
    PlayerPlatformCollision(player, &platforms[2], collision_state);
    PlayerPlatformCollision(player, &platforms[3], collision_state);
    PlayerPlatformCollision(player, &platforms[4], collision_state);
    PlayerPlatformCollision(player, &platforms[5], collision_state);
}

void FreeAndCloseResources(SDL_Surface *player_surf, SDL_Texture *player_tex,
                           SDL_Surface *block_surf, SDL_Texture *block_tex,
                           Mix_Music *music, SDL_Renderer *rend,
                           SDL_Window *win,
                           SDL_GameController *gamecontroller) {
    /* Free resources and close SDL and SDL mixer */
    Mix_FreeMusic(music);  // Free the music

    // Deallocate textues and surfaces
    SDL_FreeSurface(player_surf);    // Deallocate player and scene surfaces
    SDL_DestroyTexture(player_tex);  // Destroy scene and player textures
    SDL_FreeSurface(block_surf);     // Deallocate player and scene surfaces
    SDL_DestroyTexture(block_tex);   // Destroy scene and player textures

    // Close Game Controller
    SDL_GameControllerClose(gamecontroller);

    SDL_DestroyRenderer(rend);  // Destroy renderer
    SDL_DestroyWindow(win);     // Destroy window
    Mix_CloseAudio();           // Close Audio
    IMG_Quit();                 // Close Image
    SDL_Quit();                 // Quit SDL subsystems
}
