#include <os.h>
#include <SDL/SDL.h>

#include "math.h"
#include "gfx.h"

#define MAP_SIZE 64
#define TEX_SIZE 16
#define NUM_TEX 4

typedef Uint8 tex_t;

SDL_Surface *screen;

void mode7BuildLut(vec2_t* lutPtr, float planeH, float planeV, float height) {
	int screenRow;
	for (screenRow = 0; screenRow < SCREEN_HEIGHT; screenRow++) {
		float cameraV = (((float)screenRow / (float)SCREEN_HEIGHT) * 2.0 - 1.0);
		
		float rayX = planeH;
		float rayY = 1.0;
		float rayZ = planeV * cameraV;
		
		float intersectY = (height / rayZ) * rayY;
		float intersectX = -(height / rayZ) * rayX;
		
		lutPtr[screenRow].x = (Sint32)(intersectX * 65536);
		lutPtr[screenRow].y = (Sint32)(intersectY * 65536); 
	}
}

#define FAST_SETPIXEL8(x, y, color) (*((Uint8 *)screen->pixels + (x) + ((y) * SCREEN_WIDTH)) = (Uint8)(color))

void mode7Render(vec2_t* lutPtr, Uint8 tilemap[MAP_SIZE][MAP_SIZE],
				 tex_t texIdx[NUM_TEX][TEX_SIZE][TEX_SIZE], vec2_t pos, Sint32 heading) {
	int screenX, screenY;
	for (screenY = SCREEN_HEIGHT / 2; screenY < SCREEN_HEIGHT; screenY++) {
		vec2_t map = lutPtr[screenY];
		vec2_t mapXStep = {-map.x / (SCREEN_WIDTH / 2), 0};
		
		Sint32 sinHeading = sinLut(heading);
		Sint32 cosHeading = cosLut(heading);
		
		rotateVec2(&map, sinHeading, cosHeading);
		rotateVec2(&mapXStep, sinHeading, cosHeading);
		
		map.x += pos.x;
		map.y += pos.y;
		
		for (screenX = 0; screenX < SCREEN_WIDTH; screenX++) {
			Uint8 color = 0;

			if (map.x >= 0 && map.x < NUM_TO_FIX(MAP_SIZE)
			 && map.y >= 0 && map.y < NUM_TO_FIX(MAP_SIZE))
				color = texIdx[(int)tilemap[map.y >> TEX_SIZE][map.x >> TEX_SIZE]]
							  [(map.y & 0xFFFF) >> 12]
							  [(map.x & 0xFFFF) >> 12];

			map.x += mapXStep.x;
			map.y += mapXStep.y;

			FAST_SETPIXEL8(screenX, screenY, color);
		}
	}
	SDL_UpdateRect(screen, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);
}

SDL_Surface *loadTex(unsigned short *data) {
	SDL_Surface *tex, *tmp;
	tmp = nSDL_LoadImage(data);
	tex = SDL_DisplayFormat(tmp);
	SDL_FreeSurface(tmp);
	return tex;
}

int main(void) {
	Uint8 tilemap[MAP_SIZE][MAP_SIZE];
	vec2_t mode7lut[SCREEN_HEIGHT];
	nSDL_Font *font;
	SDL_Surface *texMushroom, *texFB, *texCartman, *texYoshi;
	int i, j;

	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 8, SDL_SWSURFACE);
	font = nSDL_LoadFont(NSDL_FONT_TINYTYPE, 255, 255, 255);
	texMushroom = loadTex(ntiMushroom);
	texFB = loadTex(ntiFB);
	texCartman = loadTex(ntiCartman);
	texYoshi = loadTex(ntiYoshi);
	SDL_FillRect(screen, NULL, 0);
	{
		const SDL_VideoInfo *info = SDL_GetVideoInfo();
		nSDL_DrawString(screen, font, 0, 0, "mode 7 test by fb39ca4\nnSDL %s\n%dx%d\n%d-bit/%d-bit",
						NSDL_VERSION, info->current_w, info->current_h, info->vfmt->BitsPerPixel, is_cx ? 16 : 4);
	}
	SDL_Flip(screen);

	srand(42);
	for (i = 0; i < MAP_SIZE; i++)
		for (j = 0; j < MAP_SIZE; j++)
			tilemap[i][j] = rand() % 4;
	
	tex_t textureIdx[NUM_TEX][TEX_SIZE][TEX_SIZE];

	for (i = 0; i < TEX_SIZE; i++)
		for (j = 0; j < TEX_SIZE; j++) {
			textureIdx[0][i][j] = nSDL_GetPixel(texMushroom, i, j);
			textureIdx[1][i][j] = nSDL_GetPixel(texFB, i, j);
			textureIdx[2][i][j] = nSDL_GetPixel(texCartman, i, j);
			textureIdx[3][i][j] = nSDL_GetPixel(texYoshi, i, j);
		}
	
	float planeH = 1.0;
	float planeV = 0.75;
	float height = 1;
	
	mode7BuildLut(mode7lut, planeH, planeV, height);
		
	Uint32 curTime = 0;
	Uint32 oldTime = 0;
	int fps = 0;
	int num_frames = 0;

	vec2_t pos = {NUM_TO_FIX(4), NUM_TO_FIX(4)};
	
	Sint32 heading = 0;

	while(!isKeyPressed(KEY_NSPIRE_ESC)) {
		vec2_t moveDist = {NUM_TO_FIX(0), FLOAT_TO_FIX(0.25)};

		rotateVec2(&moveDist, sinLut(heading), cosLut(heading));
		
		if (isKeyPressed(KEY_NSPIRE_8)) addVec2(&pos, &moveDist);
		if (isKeyPressed(KEY_NSPIRE_2)) subVec2(&pos, &moveDist);
		if (isKeyPressed(KEY_NSPIRE_6)) subVec2(&pos, rotateVec2(&moveDist, NUM_TO_FIX(1), 0));
		if (isKeyPressed(KEY_NSPIRE_4)) addVec2(&pos, rotateVec2(&moveDist, NUM_TO_FIX(1), 0));
		if (isKeyPressed(KEY_NSPIRE_9)) heading += 0x400;
		if (isKeyPressed(KEY_NSPIRE_7)) heading -= 0x400;

		mode7Render(mode7lut, tilemap, textureIdx, pos, heading);

		/* Draw & update FPS */
		{
			SDL_Rect tmp = {SCREEN_WIDTH - 40, 0, 40, 8};
			SDL_FillRect(screen, &tmp, 0);
			nSDL_DrawString(screen, font, tmp.x, tmp.y, "%d FPS", fps);
			SDL_UpdateRect(screen, tmp.x, tmp.y, tmp.w, tmp.h);

			curTime = SDL_GetTicks();
			if(curTime - oldTime <= 993) /* Clock set to 993 Hz by nSDL */
				++num_frames;
			else {
				fps = num_frames;
				num_frames = 0;
				oldTime = curTime;
			}
		}
	}

	nSDL_FreeFont(font);
	SDL_Quit();

	return 0;
}