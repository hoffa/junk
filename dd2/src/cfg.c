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
#include "cfg.h"

static const char def_hiscname[10][9] = {
	"ishygddt",
	"vykromod",
	"mantori",
	"dougan",
	"reid",
	"borogar",
	"shirro",
	"raven",
	"jean-paul",
	"lokh"
};

#define trim_newline(s) {if(s[strlen(s)-1]=='\n')s[strlen(s)-1]='\0';}

int
loadScore(char *path, score *hisc)
{
	FILE *f;
	int i,j;
	score swp;

	/* init to defaults */
	for(i=0;i<10;i++) {
		strncpy(hisc[i].name, def_hiscname[i], 9);
		hisc[i].stage=(11-i)/3?(11-i)/3:1;
		hisc[i].score=5000-(i*500);
	}

	f=fopen(path,"r");
	if(!f)
		return 0;

	for(i=0;i<10;i++) {
		char name[16], stage[16], score[16];
		if(fgets(name, 16, f) == NULL
		|| fgets(stage, 16, f) == NULL
		|| fgets(score, 16, f) == NULL)
			break;
		trim_newline(name);
		trim_newline(stage);
		trim_newline(score);
		strncpy(hisc[i].name, name, 9);
		hisc[i].stage = atoi(stage);
		hisc[i].score = atoi(score);
	}

	fclose(f);

	/* it's needed the score is ordered */
	for(i=0;i<9;i++)
		for(j=i;j<10;j++) {
			if(hisc[i].score<hisc[j].score) {
				swp=hisc[i];
				hisc[i]=hisc[j];
				hisc[j]=swp;
			} else
				if(hisc[i].score==hisc[j].score &&
					hisc[i].stage<hisc[j].stage) {
					swp=hisc[i];
					hisc[i]=hisc[j];
					hisc[j]=swp;
				}
		}

	return 1;
}

int
saveScore(char *path, score *hisc)
{
	FILE *f;
	int i;

	f=fopen(path,"w");
	if(!f)
		return 0;

	for(i=0;i<10;i++) {
		char name[16], stage[16], score[16];
		sprintf(name, "%s\n", hisc[i].name);
		sprintf(stage, "%d\n", hisc[i].stage);
		sprintf(score, "%d\n", hisc[i].score);
		fputs(name, f);
		fputs(stage, f);
		fputs(score, f);
	}
	fclose(f);
	return 1;
}
