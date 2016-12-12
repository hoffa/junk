#include <os.h>
#include <SDL/SDL.h>
#include "link.h"
#include "map1.h"

SDL_Surface *screen;
SDL_nFont *font;
SDL_bool done = SDL_FALSE;
int num_moves = 0;
player_t player;
map_t map;

void init(void) {
    if(SDL_Init(SDL_INIT_VIDEO) == -1) {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, NSP_BPP, SDL_SWSURFACE);
    if(screen == NULL) {
        printf("Couldn't initialize display: %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}

void quit(void) {
    SDL_FreeSurface(player.sprite);
    SDL_FreeSurface(map.tileset);
    SDL_Quit();
}

void init_player(int x, int y, dir_t direction, SDL_Surface *sprite) {
    SDL_SetColorKey(sprite,
                    SDL_SRCCOLORKEY | SDL_RLEACCEL,
                    SDL_MapRGB(screen->format, 255, 0, 255));
    player.x = x;
    player.y = y;
    player.direction = direction;
    player.sprite = sprite;
}

void init_map(char **data,
              int width, int height,
              int num_tiles,
              SDL_Surface *tileset,
              tile_attrib_t *tile_attrib) {
    nl_relocdata((unsigned *)data, height);
    map.data = data;
    map.width = width;
    map.height = height;
    map.num_tiles = num_tiles;
    map.tileset = tileset;
    map.tile_attrib = tile_attrib;
}

SDL_Surface *load_bmp(const char *file) {
    SDL_Surface *tmp, *image;
    tmp = SDL_LoadBMP(file);
    if(tmp == NULL)
        return NULL;
    image = SDL_DisplayFormat(tmp);
    SDL_FreeSurface(tmp);
    return image;
}

void draw_tile(SDL_Surface *tileset, int tile_num, int x, int y) {
    SDL_Rect src_rect, screen_pos;
    src_rect.x = tile_num * TILE_WIDTH;
    src_rect.y = 0;
    src_rect.w = TILE_WIDTH;
    src_rect.h = TILE_HEIGHT;
    screen_pos.x = x * TILE_WIDTH;
    screen_pos.y = y * TILE_HEIGHT;
    SDL_BlitSurface(tileset, &src_rect, screen, &screen_pos);
}

void draw_player(void) {
    draw_tile(player.sprite, (int)player.direction, player.x, player.y);
}

void draw_tile_map(void) {
    int i, j;
    for(i = 0; i < map.height; ++i)
        for(j = 0; j < map.width; ++j)
            draw_tile(map.tileset, map.data[i][j] - 'A', j, i);
}

void draw_info(void) {
    SDL_Rect rect = {0, 224, 320, 16};
    SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 202, 237, 244));
    SDL_nDrawString(screen, font, 4, 228, "Moves: %d", num_moves);
    SDL_nDrawString(screen,
                    font,
                    SCREEN_WIDTH - SDL_nGetStringWidth(font, NSP_NAME_FULL) - 4, 228,
                    NSP_NAME_FULL);
}

SDL_bool is_walkable(int x, int y) {
    return map.tile_attrib[(int)map.data[y][x] - 'A'].is_walkable;
}

void move_player(dir_t direction) {
    if(player.direction == direction) {
        int prev_x = player.x;
        int prev_y = player.y;
        switch(direction) {
            case UP:
                --player.y;
                break;
            case DOWN:
                ++player.y;
                break;
            case LEFT:
                --player.x;
                break;
            case RIGHT:
                ++player.x;

                break;
        }
        if(!is_walkable(player.x, player.y)) {
            player.x = prev_x;
            player.y = prev_y;
        } else
            ++num_moves;
    } else
        player.direction = direction;
}

void handle_keydown(SDLKey key) {
    switch(key) {
        case SDLK_UP:
            move_player(UP);
            break;
        case SDLK_DOWN:
            move_player(DOWN);
            break;
        case SDLK_LEFT:
            move_player(LEFT);
            break;
        case SDLK_RIGHT:
            move_player(RIGHT);
            break;
        case SDLK_RETURN:
            if(player.x == 14 && player.y == 8
            && player.direction == UP)
                num_moves = 1337;
            break;
        case SDLK_ESCAPE:
            done = SDL_TRUE;
            break;
        default:
            break;
    }
}

int main(void) {
    init();
    init_player(10, 7, DOWN, SDL_nLoadImage(image_link)); //load_bmp("Link/images/link.bmp.tns"));
    init_map(map1_data,
             MAP1_WIDTH, MAP1_HEIGHT,
             MAP1_NUM_TILES,
             SDL_nLoadImage(image_tileset), //load_bmp("Link/images/tileset.bmp.tns"),
             map1_tile_attrib);
    if(player.sprite == NULL)
        return EXIT_FAILURE;
    if(map.tileset == NULL) {
        SDL_FreeSurface(player.sprite);
        return EXIT_FAILURE;
    }
    font = SDL_nLoadFont(NSP_FONT_TINYTYPE,
                         SDL_MapRGB(screen->format, 0, 0, 0),
                         NSP_FONTCFG_DEFAULT);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    while(!done) {
        SDL_Event event;
        draw_tile_map();
        draw_player();
        draw_info();
        SDL_Flip(screen);
        SDL_WaitEvent(&event);
        switch(event.type) {
            case SDL_KEYDOWN:
                handle_keydown(event.key.keysym.sym);
                break;
            default:
                break;
        }
    }
    SDL_nFreeFont(font);
    quit();
    return EXIT_SUCCESS;
}