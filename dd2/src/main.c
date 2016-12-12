/*

    Dodgin' Diamond 2, a shot'em up arcade
    Copyright (C) 2003,2004 Juan J. Martinez <jjm@usebox.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <os.h>
#include <SDL/SDL.h>
#include "main.h"
#include "menu.h"
#include "gfx.h"

#define FPS	40

#include "control.h"
#include "SDL_plus.h"
#include "engine.h"
#include "cfg.h"

SDL_Surface *screen, *gfx;
nSDL_Font *font1, *font2;
extern pDesc player[2];
SDL_Event event;
Uint32 tick, ntick;
float scroll=0,scroll2=0;

bool pause;
Uint32 pause_tick;

extern bool boss;

score hiscore[10];

bool done;

void
gameLoop()
{
	int afterdeath=0;
	Uint32 last_frame = 0;
	int num_frames = 0;
	int cur_fps = 0;
	for(done=false,tick=SDL_GetTicks();!done && afterdeath<500;) {

		while(SDL_PollEvent(&event)) {
			if (event.type==SDL_QUIT)
				done=true;

			if(event.type==SDL_KEYDOWN) {
				if(event.key.keysym.sym==SDLK_ESCAPE) {
					done=true;
					continue;
				}
				else
				{
					if(event.key.keysym.sym==SDLK_LCTRL && pause_tick<SDL_GetTicks())
					{
						nSDL_DrawString(screen, is_cx?font1:font2, 160 - (nSDL_GetStringWidth(font1, "game paused") / 2), 82, "game paused");
						SDL_Flip(screen);
						pause=pause ? false : true;
						pause_tick=SDL_GetTicks()+200;
						continue;
					}
				}
			}
		}

		/* player control */
		if(player[0].shield) {
			control_player(&player[0]);
		}

		if(player[1].shield) {
			control_player(&player[1]);
		}

		if(pause)
			continue;
		
		/* frame rate calculation */
		ntick=SDL_GetTicks();
		if(ntick-tick>=1000/FPS)
		{
			tick=ntick;

				/* scroll here */
			{
				SDL_Rect a,b;
				if(is_cx) {
					if(scroll>0)
						scroll-=1;
					else
						scroll=SCREENH;

					b.x=20;
					b.w=SCREENW-40;
					a.x=20;

					if(!scroll) {
						a.y=0;
						b.y=204;
						b.h=SCREENH;
						SDL_BlitSurface(gfx, &b, screen, &a);
					} else {
						a.y=0;
						b.y=204+(int)scroll;
						b.h=SCREENH-(int)scroll;
						SDL_BlitSurface(gfx, &b, screen, &a);
						a.y=SCREENH-(int)scroll;
						b.y=204;
						b.h=(int)scroll;
						SDL_BlitSurface(gfx, &b, screen, &a);
					}
				} else
					SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255, 255, 255));

				/* scroll parallax here */

				if(scroll2>0)
					scroll2-=4;
				else
					scroll2=SCREENH;

				b.x=324;
				b.w=25;
				a.x=0;

				if(!scroll2) {
					a.y=0;
					b.y=204;
					b.h=SCREENH;
					SDL_BlitSurface(gfx, &b, screen, &a);

					b.x=358;
					a.x=SCREENW-25;
					SDL_BlitSurface(gfx, &b, screen, &a);		  		
				} else {
					a.y=0;
					b.y=204+(int)scroll2;
					b.h=SCREENH-(int)scroll2;
					SDL_BlitSurface(gfx, &b, screen, &a);
					a.y=SCREENH-(int)scroll2;
					b.y=204;
					b.h=(int)scroll2;
					SDL_BlitSurface(gfx, &b, screen, &a);

					b.x=358;
					a.x=SCREENW-25;
					a.y=0;
					b.y=204+(int)scroll2;
					b.h=SCREENH-(int)scroll2;
					SDL_BlitSurface(gfx, &b, screen, &a);
					a.y=SCREENH-(int)scroll2;
					b.y=204;
					b.h=(int)scroll2;
					SDL_BlitSurface(gfx, &b, screen, &a);	
				}
			}
			/* enemy here */
			engine_enemy();

			/* fire here */
			engine_fire();

			/* character here */
			if(player[0].shield)
				engine_player(&player[0]);

			if(player[1].shield)
				engine_player(&player[1]);

			if(!(player[0].shield | player[1].shield))
				afterdeath++;
				
			engine_obj();

			engine_vefx();

			/* panel */
			drawPanel(gfx,screen,player);

			SDL_Flip(screen);
		}
	}
}

int
main (void)
{
    int i,j,k;
    char buffer[512];
	unsigned char bpp= is_cx ? 16 : 8;

	/* read hi-scores */
	sprintf(buffer,"%s/dd2-hiscore.tns",DD2_DATA);
	if(!loadScore(buffer,hiscore))
		fprintf(stderr,"unable to read hi-scores, using defaults\n");

	i=SDL_Init(SDL_INIT_VIDEO);
	if(i<0) {
		fprintf(stderr,"Unable to init SDL: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	screen=SDL_SetVideoMode(SCREENW, SCREENH, bpp, SDL_SWSURFACE);
	if(!screen) {
		fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
		QUIT();
	}

	/* hide the mouse */
	SDL_ShowCursor(SDL_DISABLE);

	font1 = nSDL_LoadFont(NSDL_FONT_TINYTYPE, SDL_MapRGB(screen->format, 255, 255, 255), NSDL_FONTCFG_DEFAULT);
	if(!font1) {
		fprintf(stderr, "Unable to load font\n");
		QUIT();
	}

	font2 = nSDL_LoadFont(NSDL_FONT_TINYTYPE, SDL_MapRGB(screen->format, 239, 0, 0), NSDL_FONTCFG_DEFAULT);
	if(!font2) {
		fprintf(stderr, "Unable to load font\n");
		nSDL_FreeFont(font2);
		QUIT();
	}

	/* load console gfx */
	//sprintf(buffer,"%s/gfx.bmp.tns",DD2_DATA);
	gfx=nSDL_LoadImage(image_gfx);
	//gfx=loadBMP(buffer);
	if(!gfx) {
		fprintf(stderr,"Unable load gfx: %s\n", SDL_GetError());
		nSDL_FreeFont(font1);
		nSDL_FreeFont(font2);
		QUIT();
	}
	/* set transparent color */
	if(SDL_SetColorKey(gfx, SDL_SRCCOLORKEY, SDL_MapRGB(gfx->format, 255, 0, 255))<0) {
		fprintf(stderr,"Unable to setup gfx: %s\n", SDL_GetError());
		QUIT();
	}

	/* main LOOP */
	while(menu()) {
		/* init the engine */
		engine_init();

		SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,0,0,0));
		SDL_Flip(screen);

		pause=0;
		pause_tick=0;
		boss=0;
		gameLoop();

		for(i=0;i<2;i++) {
			/* check if there's a place for this score */
			for(j=9;j>=0 && hiscore[j].score<(int)player[i].score;j--);

			/* the player will be in the hall of fame? */
			if(j<9) {
				for(k=8;k>j;k--)
					hiscore[k+1]=hiscore[k];

				/* put the new score */
				hiscore[j+1].score=player[i].score;
				hiscore[j+1].stage=player[i].stage;

				hiscore[j+1].name[0]=0;
				if(!getName(hiscore[j+1].name, j+2,i+1))
					break; /* probably a problem if the user closes the window */

				/* show the hall of fame */
				hiscores();
			}
		}
	}

	/* free all! */
	SDL_FreeSurface(gfx);
	nSDL_FreeFont(font1);
	nSDL_FreeFont(font2);

	/* free engine memory */
	engine_release();

	/* save hi-scores */
	sprintf(buffer,"%s/dd2-hiscore.tns",DD2_DATA);
	if(!saveScore(buffer,hiscore))
		fprintf(stderr,"unable to save hi-scores\ndo you have permissions to write into %s?\n"
			,buffer);

	QUIT();
	return EXIT_SUCCESS;
}
