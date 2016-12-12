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

#include <SDL/SDL.h>
#include "menu.h"
#include "engine.h"
#include "cfg.h"
#include "control.h"
#include "SDL_plus.h"

extern SDL_Surface *screen, *gfx;
extern nSDL_Font *font1, *font2;

extern pDesc player[2];

extern score hiscore[10];

extern float scroll,scroll2;

void
drawGetName(char *name, int place, int playern)
{
	char buffer[64];

	/* erase the screen */
	SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,0,0,0));

	nSDL_DrawString(screen, font1, 160-(nSDL_GetStringWidth(font1, "CONGRATULATIONS")/2), 35, "CONGRATULATIONS");

	sprintf(buffer,"player %i with score %.6li",playern,player[playern-1].score);
	writeCString(gfx, screen, 10, 80, buffer, is_cx ? 1 : 0);

	switch(place) {
		default:
			sprintf(buffer,"you got %ith place",place);
		break;
		case 1:
			sprintf(buffer,"you got %ist place",place);
		break;
		case 2:
			sprintf(buffer,"you got %ind place",place);
		break;
		case 3:
			sprintf(buffer,"you got %ird place",place);
		break;
	}
	writeCString(gfx, screen, 10, 92, buffer, is_cx ? 1 : 0);

	writeCString(gfx, screen, 10, 116, "enter your name", 0);

	sprintf(buffer,"%s+",name);

	writeCString(gfx, screen, 175, 116, buffer, is_cx ? 1 : 0);

	SDL_Flip(screen);
}

int
getName(char *name, int place, int playern)
{
	SDL_Event mevent;
	int pos=0;
	char ckey='a';
	name[0] = 'a';
	name[1] = '\0';

	drawGetName(name,place,playern);

	while(1) {
		while(SDL_PollEvent(&mevent)) {
			if (mevent.type==SDL_QUIT)
				return 0;
		
			if(mevent.type==SDL_KEYDOWN) {
				if(mevent.key.keysym.sym==SDLK_ESCAPE) {
					if(name[0])
						return 1;
				}

				if(mevent.key.keysym.sym==SDLK_DOWN) {
					ckey--;

					if(ckey<'0')
						ckey='z';
					if(ckey+1=='a')
						ckey='9';

					name[pos]=ckey;
					name[pos+1]=0;
					drawGetName(name,place,playern);

					continue;
				}

				if(mevent.key.keysym.sym==SDLK_UP) {
					ckey++;

					if(ckey>'z')
						ckey='0';
					if(ckey-1=='9')
						ckey='a';

					name[pos]=ckey;
					name[pos+1]=0;
					drawGetName(name,place,playern);

					continue;
				}
			
				if(mevent.key.keysym.sym==SDLK_RIGHT) {
					if(pos<8) {
						name[pos]=ckey;
						pos++;
						name[pos]='a';
						name[pos+1]='\0';
						drawGetName(name,place,playern);

						ckey='a';
						continue;
					}
				}
				
				if(mevent.key.keysym.sym==SDLK_RETURN) {
					if(name[0]) {
						return 1;
					}
				}

				if(mevent.key.keysym.sym==SDLK_BACKSPACE) {
						if(pos>0) {
							pos--;
							if(pos)
								name[pos]=0;
							drawGetName(name,place,playern);
						}
						continue;
				}

				/* I don't know if this will work ever, in my system does */
				if((mevent.key.keysym.sym>=SDLK_a &&
					mevent.key.keysym.sym<=SDLK_z) ||
					(mevent.key.keysym.sym>=SDLK_0 &&
					mevent.key.keysym.sym<=SDLK_9)) {
					if(pos<8) {
						name[pos]=SDLK2ascii(mevent.key.keysym.sym);
						pos++;
						name[pos]=0;
						drawGetName(name,place,playern);

						ckey='a';
					}
				}
			}
		}
	}

	return 0;
}

void
drawHiscores(int max)
{
	int i;

	/* erase the screen */
	SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,0,0,0));

	/* header */
	writeCString(gfx, screen, 121, 18, "hall of fame", 1);

	for(i=0;i<max;i++) {
		writeNumber(gfx, screen, 17, 45+i*18, i+1, 2);
		writeNumber(gfx, screen, 200, 45+i*18, hiscore[i].stage,2);
		writeNumber(gfx, screen, 260, 45+i*18, hiscore[i].score,6);

		writeCString(gfx, screen, 42, 48+i*18, hiscore[i].name, 0);
		writeCString(gfx, screen, 180, 48+i*18, "st",0);
		writeCString(gfx, screen, 236, 48+i*18, "sc",0);
	}

	SDL_Flip(screen);
}

int
hiscores()
{
	SDL_Event mevent;
	int i;

	for(i=0;i<10;i++) {
		drawHiscores(i+1);
		SDL_Delay(300);
	}

	while(1) {
		while(SDL_PollEvent(&mevent)) {
    		if (mevent.type==SDL_QUIT)
    			return 0;
			if(mevent.type==SDL_KEYDOWN) {
				return 1;
			}
		}
	}

	return 0;
}

void
drawMenu(int option)
{
	SDL_Rect a,b;

	/* erase the screen */
	SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,0,0,0));

	/* BETA */
	a.x=13;
	a.y=8;
	b.x=386;
	b.y=348;
	b.w=294;
	b.h=83;
	SDL_BlitSurface(gfx, &b, screen, &a);

	/* options */
	writeCString(gfx, screen, 131, 114,  "1 player", (option==1));
	writeCString(gfx, screen, 131, 126,  "2 players", (option==2));
	writeCString(gfx, screen, 131, 150, "hall of fame", (option==3));
	writeCString(gfx, screen, 131, 162, "exit game", (option==4));
	writeCString(gfx, screen, 19, 198, "dodgin' diamond 2x (" VERSION ") by christoffer rehn", 0);
	writeCString(gfx, screen, 57, 210, "original game by juan j. martinez", 0);
	writeCString(gfx, screen, 44, 222, "uses nSDL " NSDL_VERSION " (hoffa.github.com/nsdl)", 0);

	SDL_Flip(screen);
}

int
menu()
{
	SDL_Event mevent;
	int option=1;

	drawMenu(option);

	/* some dirty init */
	scroll=scroll2=0;

	while(1) {
		while(SDL_PollEvent(&mevent)) {
			if (mevent.type==SDL_QUIT)
				return 0;
			
			if(mevent.type==SDL_KEYDOWN) {
				if(mevent.key.keysym.sym==SDLK_ESCAPE)
					return 0;
				if(mevent.key.keysym.sym==SDLK_DOWN) {
					option++;
					if(option>4)
						option=1;
					drawMenu(option);
				}
				if(mevent.key.keysym.sym==SDLK_UP) {
					option--;
					if(option<1)
						option=4;
					drawMenu(option);
				}
				if(mevent.key.keysym.sym==SDLK_RETURN) {
					switch(option) {
						default:
						break;
						case 1:
							player[0].shield=10;
							player[1].shield=0;
							player[0].score=player[1].score=0;
							player[0].stage=player[1].stage=0;
							return 1;
						case 2:
							player[0].shield=10;
							player[1].shield=10;
							player[0].score=player[1].score=0;
							player[0].stage=player[1].stage=0;
							return 1;
						case 3:
							if(!hiscores())
								return 0;
							drawMenu(option);
						break;
						case 4:
							return 0;
						break;
					}
				}
			}
		}
	}

	return 0;
}