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

#define _ENGINE_CORE_

#include <os.h>
#include <fdlibm.h>
#include "main.h"
#include "engine.h"
#include "SDL_plus.h"

extern SDL_Surface *screen, *gfx;
extern nSDL_Font *font1, *font2;

int firstInit=0;
bool boss=false;

pDesc player[2];

fDesc fire[256];
fDesc *ffree=fire, *fused=NULL;

vDesc vefx[256];
vDesc *vfree=vefx, *vused=NULL;

oDesc objects[16];
oDesc *ofree=objects, *oused=NULL;

eDesc enemy[64];
eDesc *efree=enemy, *eused=NULL;

long int actCnt=0, actIdx=0;
int act_count = 0;
extern bool done;

struct actionStruct {
	long int	a;
	int			type;
	int			x,y;
} *act=NULL;


unsigned int lfsr113(void)
{
   static unsigned int z1 = 12345, z2 = 12345, z3 = 12345, z4 = 12345;
   unsigned int b;
   b  = ((z1 << 6) ^ z1) >> 13;
   z1 = ((z1 & 4294967294U) << 18) ^ b;
   b  = ((z2 << 2) ^ z2) >> 27; 
   z2 = ((z2 & 4294967288U) << 2) ^ b;
   b  = ((z3 << 13) ^ z3) >> 21;
   z3 = ((z3 & 4294967280U) << 7) ^ b;
   b  = ((z4 << 3) ^ z4) >> 12;
   z4 = ((z4 & 4294967168U) << 13) ^ b;
   return (z1 ^ z2 ^ z3 ^ z4);
}

void
engine_init()
{
	int i;

	/* that should be init once */
	if(!firstInit) {
		player[0].f=(SDL_Rect *)malloc(sizeof(SDL_Rect)*2);
		if(!player[0].f) {
			fprintf(stderr,"ENGINE_ERROR: memory\n");
			QUIT();
		}
		player[1].f=(SDL_Rect *)malloc(sizeof(SDL_Rect)*2);
		if(!player[1].f) {
			fprintf(stderr,"ENGINE_ERROR: memory\n");
			QUIT();
		}
		player[0].f[0].x=281;
		player[0].f[0].y=134;
		player[0].f[0].w=21;
		player[0].f[0].h=27;

		player[0].f[1].x=305;
		player[0].f[1].y=134;
		player[0].f[1].w=21;
		player[0].f[1].h=27;

		player[0].f[2].x=329;
		player[0].f[2].y=134;
		player[0].f[2].w=21;
		player[0].f[2].h=27;

		player[0].nf=2;
		/* player[0].joy=0; */

		player[0].keys[0]=SDLK_7;
		player[0].keys[1]=SDLK_8;
		player[0].keys[2]=SDLK_9;
		player[0].keys[3]=SDLK_4;
		player[0].keys[4]=SDLK_6;
		player[0].keys[5]=SDLK_1;
		player[0].keys[6]=SDLK_2;
		player[0].keys[7]=SDLK_3;
		player[0].keys[8]=SDLK_5;

		player[1].f[0].x=281;
		player[1].f[0].y=164;
		player[1].f[0].w=21;
		player[1].f[0].h=27;

		player[1].f[1].x=305;
		player[1].f[1].y=164;
		player[1].f[1].w=21;
		player[1].f[1].h=27;

		player[1].f[2].x=329;
		player[1].f[2].y=164;
		player[1].f[2].w=21;
		player[1].f[2].h=27;

		player[1].nf=2;
		/* player[1].joy=0; */

		player[1].keys[0]=SDLK_e;
		player[1].keys[1]=SDLK_f;
		player[1].keys[2]=SDLK_g;
		player[1].keys[3]=SDLK_l;
		player[1].keys[4]=SDLK_n;
		player[1].keys[5]=SDLK_s;
		player[1].keys[6]=SDLK_t;
		player[1].keys[7]=SDLK_u;
		player[1].keys[8]=SDLK_m;

		firstInit=1;
	}

/* now follows the stuff that must be setup each play */
	
	player[0].cf=0;
	player[0].score=0;
	//player[0].shield=0;
	player[0].weapon=0;
	player[0].level=0;
	player[0].h_dir=0;
	player[0].x=100;
	player[0].y=160;
	player[0].incx=0;
	player[0].incy=0;
	player[0].fire=0;
	player[0].firef=0;	
	player[0].ftime=2;
	player[0].cftime=0;

	player[0].companion=0;
	player[0].t=0;
	player[0].cinc=1;

/* ********************* */

	player[1].cf=0;
	player[1].score=0;
	//player[1].shield=0;
	player[1].weapon=0;
	player[1].level=0;
	player[1].h_dir=0;
	player[1].x=200;
	player[1].y=160;
	player[1].incx=0;
	player[1].incy=0;
	player[1].fire=0;
	player[1].firef=0;
	player[1].ftime=2;
	player[1].cftime=0;

	player[1].companion=0;
	player[1].t=0;
	player[1].cinc=1;

/* ********************* */

	for(i=0;i<63;i++)
		enemy[i].n=&enemy[i+1];
	enemy[i].n=NULL;

	for(i=0;i<255;i++)
	{
		fire[i].n=&fire[i+1];
		vefx[i].n=&vefx[i+1];
	}
	fire[i].n=NULL;
	vefx[i].n=NULL;

	for(i=0;i<15;i++)
		objects[i].n=&objects[i+1];
	objects[i].n=NULL;

	ffree=fire;
	fused=NULL;

	vfree=vefx;
	vused=NULL;

	ofree=objects;
	oused=NULL;

	efree=enemy;
	eused=NULL;
	
	actCnt=0;
	actIdx=0;
	
	if(act)
		free(act);

	act_count = 221;
	act=(struct actionStruct *)malloc(sizeof(struct actionStruct)*act_count);
	if(!act) {
		fprintf(stderr,"ENGINE_ERROR: memory\n");
		QUIT();
	}
	
	act[0].a=10;act[0].type=0;act[0].x=6;act[0].y=1;
	act[1].a=300;act[1].type=3;act[1].x=200;act[1].y=-32;
	act[2].a=20;act[2].type=3;act[2].x=180;act[2].y=-32;
	act[3].a=0;act[3].type=3;act[3].x=220;act[3].y=-32;
	act[4].a=200;act[4].type=3;act[4].x=80;act[4].y=-32;
	act[5].a=20;act[5].type=3;act[5].x=60;act[5].y=-32;
	act[6].a=0;act[6].type=3;act[6].x=100;act[6].y=-32;
	act[7].a=200;act[7].type=3;act[7].x=200;act[7].y=-32;
	act[8].a=20;act[8].type=3;act[8].x=180;act[8].y=-32;
	act[9].a=0;act[9].type=3;act[9].x=220;act[9].y=-32;
	act[10].a=100;act[10].type=2;act[10].x=40;act[10].y=-32;
	act[11].a=30;act[11].type=2;act[11].x=40;act[11].y=-32;
	act[12].a=30;act[12].type=2;act[12].x=40;act[12].y=-32;
	act[13].a=30;act[13].type=2;act[13].x=40;act[13].y=-32;
	act[14].a=75;act[14].type=2;act[14].x=260;act[14].y=-32;
	act[15].a=30;act[15].type=2;act[15].x=260;act[15].y=-32;
	act[16].a=30;act[16].type=2;act[16].x=260;act[16].y=-32;
	act[17].a=30;act[17].type=2;act[17].x=260;act[17].y=-32;
	act[18].a=75;act[18].type=3;act[18].x=80;act[18].y=-32;
	act[19].a=20;act[19].type=3;act[19].x=60;act[19].y=-32;
	act[20].a=0;act[20].type=3;act[20].x=100;act[20].y=-32;
	act[21].a=300;act[21].type=4;act[21].x=200;act[21].y=-48;
	act[22].a=150;act[22].type=3;act[22].x=100;act[22].y=-32;
	act[23].a=20;act[23].type=3;act[23].x=80;act[23].y=-32;
	act[24].a=0;act[24].type=3;act[24].x=120;act[24].y=-32;
	act[25].a=20;act[25].type=3;act[25].x=60;act[25].y=-32;
	act[26].a=0;act[26].type=3;act[26].x=140;act[26].y=-32;
	act[27].a=200;act[27].type=2;act[27].x=40;act[27].y=-32;
	act[28].a=30;act[28].type=2;act[28].x=40;act[28].y=-32;
	act[29].a=30;act[29].type=2;act[29].x=40;act[29].y=-32;
	act[30].a=30;act[30].type=2;act[30].x=40;act[30].y=-32;
	act[31].a=30;act[31].type=2;act[31].x=40;act[31].y=-32;
	act[32].a=200;act[32].type=1;act[32].x=60;act[32].y=-32;
	act[33].a=30;act[33].type=1;act[33].x=60;act[33].y=-32;
	act[34].a=30;act[34].type=1;act[34].x=60;act[34].y=-32;
	act[35].a=30;act[35].type=1;act[35].x=60;act[35].y=-32;
	act[36].a=30;act[36].type=1;act[36].x=60;act[36].y=-32;
	act[37].a=30;act[37].type=1;act[37].x=60;act[37].y=-32;
	act[38].a=30;act[38].type=1;act[38].x=60;act[38].y=-32;
	act[39].a=30;act[39].type=1;act[39].x=60;act[39].y=-32;
	act[40].a=30;act[40].type=1;act[40].x=60;act[40].y=-32;
	act[41].a=100;act[41].type=3;act[41].x=60;act[41].y=-32;
	act[42].a=20;act[42].type=3;act[42].x=40;act[42].y=-32;
	act[43].a=0;act[43].type=3;act[43].x=80;act[43].y=-32;
	act[44].a=200;act[44].type=4;act[44].x=80;act[44].y=-48;
	act[45].a=200;act[45].type=3;act[45].x=220;act[45].y=-32;
	act[46].a=20;act[46].type=3;act[46].x=200;act[46].y=-32;
	act[47].a=0;act[47].type=3;act[47].x=240;act[47].y=-32;
	act[48].a=350;act[48].type=1;act[48].x=60;act[48].y=-32;
	act[49].a=30;act[49].type=1;act[49].x=60;act[49].y=-32;
	act[50].a=30;act[50].type=1;act[50].x=60;act[50].y=-32;
	act[51].a=30;act[51].type=1;act[51].x=60;act[51].y=-32;
	act[52].a=30;act[52].type=1;act[52].x=60;act[52].y=-32;
	act[53].a=30;act[53].type=1;act[53].x=60;act[53].y=-32;
	act[54].a=30;act[54].type=1;act[54].x=60;act[54].y=-32;
	act[55].a=30;act[55].type=1;act[55].x=60;act[55].y=-32;
	act[56].a=30;act[56].type=1;act[56].x=60;act[56].y=-32;
	act[57].a=200;act[57].type=2;act[57].x=40;act[57].y=-32;
	act[58].a=30;act[58].type=2;act[58].x=40;act[58].y=-32;
	act[59].a=30;act[59].type=2;act[59].x=40;act[59].y=-32;
	act[60].a=30;act[60].type=2;act[60].x=40;act[60].y=-32;
	act[61].a=30;act[61].type=2;act[61].x=40;act[61].y=-32;
	act[62].a=100;act[62].type=2;act[62].x=260;act[62].y=-32;
	act[63].a=30;act[63].type=2;act[63].x=260;act[63].y=-32;
	act[64].a=30;act[64].type=2;act[64].x=260;act[64].y=-32;
	act[65].a=30;act[65].type=2;act[65].x=260;act[65].y=-32;
	act[66].a=30;act[66].type=2;act[66].x=260;act[66].y=-32;
	act[67].a=150;act[67].type=1;act[67].x=60;act[67].y=-32;
	act[68].a=80;act[68].type=1;act[68].x=60;act[68].y=-32;
	act[69].a=80;act[69].type=1;act[69].x=60;act[69].y=-32;
	act[70].a=80;act[70].type=1;act[70].x=60;act[70].y=-32;
	act[71].a=80;act[71].type=1;act[71].x=60;act[71].y=-32;
	act[72].a=80;act[72].type=1;act[72].x=60;act[72].y=-32;
	act[73].a=80;act[73].type=1;act[73].x=60;act[73].y=-32;
	act[74].a=80;act[74].type=1;act[74].x=60;act[74].y=-32;
	act[75].a=80;act[75].type=1;act[75].x=60;act[75].y=-32;
	act[76].a=80;act[76].type=1;act[76].x=60;act[76].y=-32;
	act[77].a=80;act[77].type=1;act[77].x=60;act[77].y=-32;
	act[78].a=80;act[78].type=1;act[78].x=60;act[78].y=-32;
	act[79].a=80;act[79].type=1;act[79].x=60;act[79].y=-32;
	act[80].a=80;act[80].type=1;act[80].x=60;act[80].y=-32;
	act[81].a=200;act[81].type=3;act[81].x=110;act[81].y=-32;
	act[82].a=20;act[82].type=3;act[82].x=90;act[82].y=-32;
	act[83].a=0;act[83].type=3;act[83].x=130;act[83].y=-32;
	act[84].a=20;act[84].type=3;act[84].x=70;act[84].y=-32;
	act[85].a=0;act[85].type=3;act[85].x=150;act[85].y=-32;
	act[86].a=20;act[86].type=3;act[86].x=50;act[86].y=-32;
	act[87].a=0;act[87].type=3;act[87].x=170;act[87].y=-32;
	act[88].a=150;act[88].type=3;act[88].x=190;act[88].y=-32;
	act[89].a=20;act[89].type=3;act[89].x=170;act[89].y=-32;
	act[90].a=0;act[90].type=3;act[90].x=210;act[90].y=-32;
	act[91].a=20;act[91].type=3;act[91].x=150;act[91].y=-32;
	act[92].a=0;act[92].type=3;act[92].x=230;act[92].y=-32;
	act[93].a=20;act[93].type=3;act[93].x=130;act[93].y=-32;
	act[94].a=0;act[94].type=3;act[94].x=250;act[94].y=-32;
	act[95].a=20;act[95].type=4;act[95].x=80;act[95].y=-48;
	act[96].a=300;act[96].type=5;act[96].x=100;act[96].y=-32;
	act[97].a=30;act[97].type=5;act[97].x=100;act[97].y=-32;
	act[98].a=30;act[98].type=5;act[98].x=100;act[98].y=-32;
	act[99].a=30;act[99].type=5;act[99].x=100;act[99].y=-32;
	act[100].a=30;act[100].type=5;act[100].x=100;act[100].y=-32;
	act[101].a=30;act[101].type=5;act[101].x=220;act[101].y=-32;
	act[102].a=30;act[102].type=5;act[102].x=220;act[102].y=-32;
	act[103].a=30;act[103].type=5;act[103].x=220;act[103].y=-32;
	act[104].a=30;act[104].type=5;act[104].x=220;act[104].y=-32;
	act[105].a=30;act[105].type=5;act[105].x=220;act[105].y=-32;
	act[106].a=100;act[106].type=5;act[106].x=100;act[106].y=-32;
	act[107].a=30;act[107].type=5;act[107].x=100;act[107].y=-32;
	act[108].a=30;act[108].type=5;act[108].x=100;act[108].y=-32;
	act[109].a=30;act[109].type=5;act[109].x=100;act[109].y=-32;
	act[110].a=30;act[110].type=5;act[110].x=100;act[110].y=-32;
	act[111].a=30;act[111].type=5;act[111].x=220;act[111].y=-32;
	act[112].a=30;act[112].type=5;act[112].x=220;act[112].y=-32;
	act[113].a=30;act[113].type=5;act[113].x=220;act[113].y=-32;
	act[114].a=30;act[114].type=5;act[114].x=220;act[114].y=-32;
	act[115].a=30;act[115].type=5;act[115].x=220;act[115].y=-32;
	act[116].a=300;act[116].type=6;act[116].x=110;act[116].y=-60;
	act[117].a=300;act[117].type=0;act[117].x=6;act[117].y=2;
	act[118].a=20;act[118].type=7;act[118].x=40;act[118].y=-32;
	act[119].a=35;act[119].type=7;act[119].x=80;act[119].y=-32;
	act[120].a=45;act[120].type=7;act[120].x=230;act[120].y=-32;
	act[121].a=40;act[121].type=7;act[121].x=120;act[121].y=-32;
	act[122].a=30;act[122].type=7;act[122].x=200;act[122].y=-32;
	act[123].a=55;act[123].type=7;act[123].x=90;act[123].y=-32;
	act[124].a=60;act[124].type=7;act[124].x=240;act[124].y=-32;
	act[125].a=45;act[125].type=7;act[125].x=60;act[125].y=-32;
	act[126].a=30;act[126].type=7;act[126].x=150;act[126].y=-32;
	act[127].a=45;act[127].type=7;act[127].x=100;act[127].y=-32;
	act[128].a=60;act[128].type=7;act[128].x=180;act[128].y=-32;
	act[129].a=20;act[129].type=7;act[129].x=30;act[129].y=-32;
	act[130].a=20;act[130].type=7;act[130].x=260;act[130].y=-32;
	act[131].a=30;act[131].type=7;act[131].x=130;act[131].y=-32;
	act[132].a=30;act[132].type=7;act[132].x=220;act[132].y=-32;
	act[133].a=55;act[133].type=7;act[133].x=60;act[133].y=-32;
	act[134].a=60;act[134].type=7;act[134].x=140;act[134].y=-32;
	act[135].a=45;act[135].type=7;act[135].x=35;act[135].y=-32;
	act[136].a=30;act[136].type=7;act[136].x=110;act[136].y=-32;
	act[137].a=60;act[137].type=7;act[137].x=220;act[137].y=-32;
	act[138].a=30;act[138].type=7;act[138].x=160;act[138].y=-32;
	act[139].a=50;act[139].type=2;act[139].x=260;act[139].y=-32;
	act[140].a=30;act[140].type=2;act[140].x=260;act[140].y=-32;
	act[141].a=30;act[141].type=2;act[141].x=260;act[141].y=-32;
	act[142].a=30;act[142].type=2;act[142].x=260;act[142].y=-32;
	act[143].a=20;act[143].type=8;act[143].x=40;act[143].y=-32;
	act[144].a=20;act[144].type=2;act[144].x=40;act[144].y=-32;
	act[145].a=30;act[145].type=2;act[145].x=40;act[145].y=-32;
	act[146].a=30;act[146].type=2;act[146].x=40;act[146].y=-32;
	act[147].a=30;act[147].type=2;act[147].x=40;act[147].y=-32;
	act[148].a=50;act[148].type=2;act[148].x=260;act[148].y=-32;
	act[149].a=30;act[149].type=2;act[149].x=260;act[149].y=-32;
	act[150].a=30;act[150].type=2;act[150].x=260;act[150].y=-32;
	act[151].a=30;act[151].type=2;act[151].x=260;act[151].y=-32;
	act[152].a=30;act[152].type=2;act[152].x=260;act[152].y=-32;
	act[153].a=30;act[153].type=2;act[153].x=260;act[153].y=-32;
	act[154].a=30;act[154].type=2;act[154].x=260;act[154].y=-32;
	act[155].a=30;act[155].type=4;act[155].x=60;act[155].y=-48;
	act[156].a=120;act[156].type=8;act[156].x=260;act[156].y=-32;
	act[157].a=40;act[157].type=3;act[157].x=200;act[157].y=-32;
	act[158].a=20;act[158].type=3;act[158].x=180;act[158].y=-32;
	act[159].a=0;act[159].type=3;act[159].x=220;act[159].y=-32;
	act[160].a=20;act[160].type=3;act[160].x=160;act[160].y=-32;
	act[161].a=0;act[161].type=3;act[161].x=240;act[161].y=-32;
	act[162].a=100;act[162].type=8;act[162].x=40;act[162].y=-32;
	act[163].a=100;act[163].type=3;act[163].x=100;act[163].y=-32;
	act[164].a=20;act[164].type=3;act[164].x=80;act[164].y=-32;
	act[165].a=0;act[165].type=3;act[165].x=120;act[165].y=-32;
	act[166].a=20;act[166].type=3;act[166].x=60;act[166].y=-32;
	act[167].a=0;act[167].type=3;act[167].x=140;act[167].y=-32;
	act[168].a=20;act[168].type=2;act[168].x=40;act[168].y=-32;
	act[169].a=30;act[169].type=2;act[169].x=40;act[169].y=-32;
	act[170].a=30;act[170].type=2;act[170].x=40;act[170].y=-32;
	act[171].a=30;act[171].type=2;act[171].x=40;act[171].y=-32;
	act[172].a=30;act[172].type=2;act[172].x=40;act[172].y=-32;
	act[173].a=30;act[173].type=2;act[173].x=40;act[173].y=-32;
	act[174].a=20;act[174].type=8;act[174].x=260;act[174].y=-32;
	act[175].a=20;act[175].type=9;act[175].x=130;act[175].y=-42;
	act[176].a=180;act[176].type=9;act[176].x=40;act[176].y=-42;
	act[177].a=0;act[177].type=9;act[177].x=220;act[177].y=-42;
	act[178].a=80;act[178].type=8;act[178].x=260;act[178].y=-32;
	act[179].a=80;act[179].type=8;act[179].x=40;act[179].y=-32;
	act[180].a=230;act[180].type=9;act[180].x=80;act[180].y=-42;
	act[181].a=0;act[181].type=9;act[181].x=180;act[181].y=-42;
	act[182].a=120;act[182].type=8;act[182].x=260;act[182].y=-32;
	act[183].a=120;act[183].type=8;act[183].x=40;act[183].y=-32;
	act[184].a=20;act[184].type=9;act[184].x=130;act[184].y=-42;
	act[185].a=300;act[185].type=4;act[185].x=130;act[185].y=-48;
	act[186].a=150;act[186].type=2;act[186].x=260;act[186].y=-32;
	act[187].a=30;act[187].type=2;act[187].x=260;act[187].y=-32;
	act[188].a=30;act[188].type=2;act[188].x=260;act[188].y=-32;
	act[189].a=30;act[189].type=2;act[189].x=260;act[189].y=-32;
	act[190].a=100;act[190].type=3;act[190].x=60;act[190].y=-32;
	act[191].a=20;act[191].type=3;act[191].x=40;act[191].y=-32;
	act[192].a=0;act[192].type=3;act[192].x=80;act[192].y=-32;
	act[193].a=200;act[193].type=5;act[193].x=220;act[193].y=-32;
	act[194].a=30;act[194].type=5;act[194].x=220;act[194].y=-32;
	act[195].a=30;act[195].type=5;act[195].x=220;act[195].y=-32;
	act[196].a=30;act[196].type=11;act[196].x=260;act[196].y=232;
	act[197].a=30;act[197].type=5;act[197].x=220;act[197].y=-32;
	act[198].a=30;act[198].type=5;act[198].x=100;act[198].y=-32;
	act[199].a=30;act[199].type=11;act[199].x=260;act[199].y=232;
	act[200].a=30;act[200].type=5;act[200].x=100;act[200].y=-32;
	act[201].a=30;act[201].type=5;act[201].x=100;act[201].y=-32;
	act[202].a=30;act[202].type=5;act[202].x=100;act[202].y=-32;
	act[203].a=300;act[203].type=4;act[203].x=130;act[203].y=-48;
	act[204].a=10;act[204].type=0;act[204].x=6;act[204].y=3;
	act[205].a=300;act[205].type=11;act[205].x=260;act[205].y=232;
	act[206].a=30;act[206].type=11;act[206].x=260;act[206].y=232;
	act[207].a=30;act[207].type=11;act[207].x=260;act[207].y=232;
	act[208].a=100;act[208].type=11;act[208].x=40;act[208].y=232;
	act[209].a=30;act[209].type=11;act[209].x=40;act[209].y=232;
	act[210].a=30;act[210].type=11;act[210].x=40;act[210].y=232;
	act[211].a=60;act[211].type=8;act[211].x=260;act[211].y=-32;
	act[212].a=80;act[212].type=11;act[212].x=40;act[212].y=232;
	act[213].a=30;act[213].type=11;act[213].x=40;act[213].y=232;
	act[214].a=30;act[214].type=11;act[214].x=40;act[214].y=232;
	act[215].a=100;act[215].type=11;act[215].x=260;act[215].y=232;
	act[216].a=30;act[216].type=11;act[216].x=260;act[216].y=232;
	act[217].a=30;act[217].type=11;act[217].x=260;act[217].y=232;
	act[218].a=60;act[218].type=8;act[218].x=40;act[218].y=-32;
	act[219].a=300;act[219].type=10;act[219].x=65;act[219].y=-52;
	act[220].a=-1;act[220].type=0;act[220].x=0;act[220].y=0;
}

void
engine_release()
{
	int i;

	for(i=0;i<2;i++)
		if(player[i].f)
			free(player[i].f);
			
	if(act)
		free(act);
}

void
engine_obj()
{
	oDesc *ot,*otp;
	SDL_Rect a,b;
	int i;
	
	/* process all the objects */
	for(otp=ot=oused;ot;) {
		ot->y+=0.25;
		if(ot->ftime)
			ot->ftime--;
		else {
			ot->ftime=10;
			ot->cf=ot->cf ? 0 : 1;
			if(ot->y>SCREENH) {
				if(ot==oused) {
					otp=ofree;
					ofree=oused;
					oused=oused->n;
					ofree->n=otp;
					otp=ot=oused;
					continue;					
				} else {
					otp->n=ot->n;
					ot->n=ofree;
					ofree=ot;
					ot=otp->n;
					continue;
				}
			}
		}

		a.x=ot->x;
		a.y=ot->y;
		b.w=20;
		b.h=20;
		
		switch(ot->type) {
			default:
			case OBJ_SHIELD:
				b.x=200;
				b.y=24;
			break;
			case OBJ_WEAPON1:
				b.x=90;
				b.y=24;
			break;
			case OBJ_WEAPON2:
				b.x=112;
				b.y=24;
			break;
			case OBJ_WEAPON3:
				b.x=134;
				b.y=24;
			break;
			case OBJ_COMPANION:
				b.x=178;
				b.y=24;
			break;
		}

		SDL_BlitSurface(gfx, &b, screen, &a);

		for(i=0;i<2;i++)
			if(player[i].shield>0)
				if(ot->x+10>player[i].x && ot->x+10<player[i].x+20 &&
					ot->y+10>player[i].y && ot->y+10<player[i].y+20) {
						switch(ot->type) {
							default:
							break;
							case OBJ_SHIELD:
								if(player[i].shield<10) {
									player[i].shield+=3;
									if(player[i].shield>10)
										player[i].shield=10;
									engine_add_vefx(VFX_SHUP,ot->x,ot->y);
								} else {
									player[i].score+=300;
								}
							break;
							case OBJ_WEAPON1:
								if(player[i].weapon!=0) {
									player[i].weapon=0;
									player[i].level--;
									if(player[i].level<0)
										player[i].level=0;
									engine_add_vefx(VFX_POW,ot->x,ot->y);
								} else {
									if(player[i].level<4) {
										player[i].level++;
										engine_add_vefx(VFX_POW,ot->x,ot->y);
									} else {
										player[i].score+=100;
									}
								}
							break;
							case OBJ_WEAPON2:
								if(player[i].weapon!=1) {
									player[i].weapon=1;
									player[i].level--;
									if(player[i].level<0)
										player[i].level=0;
									engine_add_vefx(VFX_POW,ot->x,ot->y);
								} else {
									if(player[i].level<4) {
										player[i].level++;
										engine_add_vefx(VFX_POW,ot->x,ot->y);
									} else {
										player[i].score+=100;
									}
								}
							break;
							case OBJ_WEAPON3:
								if(player[i].weapon!=2) {
									player[i].weapon=2;
									player[i].level--;
									if(player[i].level<0)
										player[i].level=0;
									engine_add_vefx(VFX_POW,ot->x,ot->y);
								} else {
									if(player[i].level<4) {
										player[i].level++;
										engine_add_vefx(VFX_POW,ot->x,ot->y);
									} else {
										player[i].score+=100;
									}
								}
							break;
							case OBJ_COMPANION:
								if(!player[i].companion) {
									player[i].companion=1;
									player[i].t=0;
									player[i].cinc=1;
									circle_path(player[i].x+10,player[i].y+10,30,player[i].t,&player[i].cx,&player[i].cy);
									engine_add_vefx(VFX_POW,ot->x,ot->y);
								} else {
									player[i].score+=250;
								}
							break;
						}
						ot->y=SCREENW+20;
				}

		otp=ot;
		ot=ot->n;
	}
}

void
engine_add_obj(int type, int x, int y)
{
	oDesc *ot;

	if(ofree==NULL) {
		fprintf(stderr,"PANIC!!!! ofree reached limit\n");
		QUIT();
	}
	
	if(oused==NULL) {
		oused=ofree;
		ofree=ofree->n;
		oused->n=NULL;
	} else {
		ot=oused;
		oused=ofree;
		ofree=ofree->n;
		oused->n=ot;
	}
	
	oused->type=type;
	oused->x=x;
	oused->y=y;
	oused->ftime=10;
	oused->cf=0;
}

void
engine_vefx()
{
	vDesc *vt,*vtp;
	SDL_Rect a,b;

	/* process all the efx */
	for(vtp=vt=vused;vt;) {
		if(vt->cftime<vt->ftime)
			vt->cftime++;
		else {
			vt->cftime=0;
			if(vt->cf+1==vt->nf) {
				if(vt==vused) {
					vtp=vfree;
					vfree=vused;
					vused=vused->n;
					vfree->n=vtp;
					vtp=vt=vused;
					continue;
				} else {
					vtp->n=vt->n;
					vt->n=vfree;
					vfree=vt;
					vt=vtp->n;
					continue;
				}
			} else
				vt->cf++;
		}

		if(vt->type!=VFX_STAGE) {
			a.x=vt->x;
			a.y=vt->y;
			b.x=vt->f[vt->cf].x;
			b.y=vt->f[vt->cf].y;
			b.w=vt->f[vt->cf].w;
			b.h=vt->f[vt->cf].h;
			SDL_BlitSurface(gfx, &b, screen, &a);
		} else { /* just the STAGE N banner */
			char stage[16];
			sprintf(stage,"stage %i",vt->y);
			nSDL_DrawString(screen, is_cx?font1:font2, 160 - (nSDL_GetStringWidth(font1, stage) / 2), 82, stage);
		}

		vtp=vt;
		vt=vt->n;
	}
}

void
engine_add_vefx(int type, int x, int y)
{
	vDesc *vt;

	if(vfree==NULL) {
		fprintf(stderr,"PANIC!!!! vfree reached limit\n");
		QUIT();
	}

	if(vused==NULL) {
		vused=vfree;
		vfree=vfree->n;
		vused->n=NULL;
	} else {
		vt=vused;
		vused=vfree;
		vfree=vfree->n;
		vused->n=vt;
	}

	vused->type=type;
	vused->x=x;
	vused->y=y;
	vused->cftime=0;

	switch(type) {
		default:
		case VFX_SHIELD:
			vused->ftime=4;
			vused->nf=4;
			vused->cf=0;

			vused->f[0].x=46;
			vused->f[0].y=2;
			vused->f[0].w=20;
			vused->f[0].h=20;
			vused->f[1]=vused->f[0];
			vused->f[1].x=68;
			vused->f[2]=vused->f[0];
			vused->f[2].x=90;
			vused->f[3]=vused->f[0];
			vused->f[3].x=112;
		break;
		case VFX_EXPLO:
			vused->ftime=4;
			vused->nf=4;
			vused->cf=0;

			vused->f[0].x=134;
			vused->f[0].y=2;
			vused->f[0].w=20;
			vused->f[0].h=20;
			vused->f[1]=vused->f[0];
			vused->f[1].x=156;
			vused->f[2]=vused->f[0];
			vused->f[2].x=178;
			vused->f[3]=vused->f[0];
			vused->f[3].x=200;
		break;
		case VFX_EXPLOB:
			vused->ftime=4;
			vused->nf=3;
			vused->cf=0;

			vused->f[0].x=222;
			vused->f[0].y=2;
			vused->f[0].w=20;
			vused->f[0].h=20;
			vused->f[1]=vused->f[0];
			vused->f[1].x=244;
			vused->f[2]=vused->f[0];
			vused->f[2].x=266;
		break;
		case VFX_SHUP:
			vused->ftime=80;
			vused->nf=1;
			vused->cf=0;

			vused->f[0].x=222;
			vused->f[0].y=24;
			vused->f[0].w=45;
			vused->f[0].h=10;
		break;
		case VFX_POW:
			vused->ftime=80;
			vused->nf=1;
			vused->cf=0;

			vused->f[0].x=222;
			vused->f[0].y=35;
			vused->f[0].w=45;
			vused->f[0].h=10;
		break;
		case VFX_STAGE: /* special vefx */
			vused->ftime=250;
			vused->nf=1;
			vused->cf=0;
			if(player[0].shield>0)
				player[0].stage++;
			if(player[1].shield>0)
				player[1].stage++;
		break;
		case VFX_MEXPLO:
			engine_add_vefx(VFX_EXPLO, x-20, y);
			engine_add_vefx(VFX_EXPLO, x, y-20);
			engine_add_vefx(VFX_EXPLO, x, y+20);
			engine_add_vefx(VFX_EXPLO, x+20, y);
		break;

	}
}

void
engine_add_fire(int from, int type, int x, int y, float incx, float incy, pDesc *p)
{
	fDesc *ft;

	if(ffree==NULL) {
		fprintf(stderr,"PANIC!!!! ffree reached limit\n");
		QUIT();
	}

	if(fused==NULL) {
		fused=ffree;
		ffree=ffree->n;
		fused->n=NULL;
	} else {
		ft=fused;
		fused=ffree;
		ffree=ffree->n;
		fused->n=ft;
	}

	fused->own=from;
	fused->type=type;
	fused->x=x;
	fused->y=y;
	fused->incx=incx;
	fused->incy=incy;
	fused->cftime=0;
	fused->player=p;

	/* set speed */
	switch(type) {
		default:
		case 0:
			fused->damage=1;
			fused->incx*=8;
			fused->incy*=8;
			fused->ftime=2;
		break;
		case 3:
		case 1:
			fused->damage=2;
			fused->incx*=10;
			fused->incy*=10;
			fused->ftime=2;
		break;
		case 2:
			fused->damage=3;
			fused->incx*=12;
			fused->incy*=12;
			fused->ftime=2;
		break;
		case 5:
		case 4:
			fused->damage=1;
			fused->incx*=4;
			fused->incy*=4;
			fused->ftime=2;
		break;
		case 6:
			fused->damage=1;
			fused->incx*=5;
			fused->incy*=5;
			fused->ftime=2;
		break;
	}
}

void
engine_fire()
{
	fDesc *ft,*ftp;
	eDesc *e;
	oDesc *o;
	SDL_Rect a,b;
	int i,j;

	/* process all the fires */
	for(ftp=ft=fused;ft;) {
		if(ft->cftime<ft->ftime)
			ft->cftime++;
		else {
			ft->cftime=0;
			ft->x+=ft->incx;
			ft->y+=ft->incy;
		}
		/* check if goes out the screen */
		if(ft->x<0 || ft->x>SCREENW
		   || ft->y<0 || ft->y>SCREENH) {
				if(ft==fused) {
					ftp=ffree;
					ffree=fused;
					fused=fused->n;
					ffree->n=ftp;
					ftp=ft=fused;
					continue;
				} else {
					ftp->n=ft->n;
					ft->n=ffree;
					ffree=ft;
					ft=ftp->n;
					continue;
				}
		}
		/* walls */
		if(ft->x<30 || ft->x>SCREENW-25) {
			engine_add_vefx(VFX_EXPLOB,ft->x-12,ft->y-12);
			ft->y=-20;
		}
		/* players */
		if(ft->own!=1) {
			for(i=0,j=0;i<2;i++)
				if(player[i].shield>0)
					if(ft->x>player[i].x && ft->x<player[i].x+20 &&
						ft->y>player[i].y && ft->y<player[i].y+20) {
							engine_add_vefx(VFX_EXPLOB,ft->x-12,ft->y-12);
							j++;
							player[i].shield--;
							if(player[i].shield>0)
								engine_add_vefx(VFX_SHIELD,player[i].x,player[i].y);
							else
								engine_add_vefx(VFX_EXPLO,player[i].x,player[i].y);
					}
			if(j)
				ft->y=-10;
		}
		/* enemies */
		if(ft->own!=2) {
			for(e=eused; e; e=e->n) {
				if(e->shield>0) {
					if((e->hit)(e,ft->x+2,ft->y+2)) {
							engine_add_vefx(VFX_EXPLOB,ft->x-12,ft->y-12);
														
							ft->y=-20;
							e->shield-=ft->damage;
							if(e->shield<=0) {
								engine_add_vefx(VFX_EXPLO,e->x+6,e->y+6);
								ft->player->score+=e->score;

								if(e->type==6)
									engine_add_vefx(VFX_MEXPLO,e->x+46,e->y+26);
								if(e->type==10)
									engine_add_vefx(VFX_MEXPLO,e->x+55,e->y+26);
							} 
							else
								ft->player->score+=e->score/(ft->player->level+1);
					}
				}
			}
		}
		/* object change */
		if(ft->own!=2) {
			for(o=oused; o; o=o->n)
				if(ft->x+2>o->x && ft->x+2<o->x+20 &&
					ft->y+2>o->y && ft->y+2<o->y+20) {
					if(o->type<OBJ_LAST)
						o->type++;
					else
						o->type=OBJ_FIRST;
					engine_add_vefx(VFX_EXPLOB,ft->x-12,ft->y-12);
					ft->y=-20;
				}
		}

		/* companion */
		if(ft->own!=1) {
			for(i=0;i<2;i++)
				if(player[i].shield>0 && player[i].companion)
					if(ft->x+2>player[i].cx && ft->x+2<player[i].cx+4 &&
						ft->y+2>player[i].cy && ft->y+2<player[i].cy+4) {
						engine_add_vefx(VFX_EXPLOB,player[i].cx-10,player[i].cy-10);
						player[i].companion=0;
					}
		}

		switch(ft->type) {
			default:
			case 0:
				b.x=47;
				b.y=24;
				b.w=5;
				b.h=10;
			break;
			case 1:
				b.x=53;
				b.y=24;
				b.w=4;
				b.h=10;
			break;
			case 2:
				b.x=59;
				b.y=24;
				b.w=5;
				b.h=10;
			break;
			case 3:
				b.x=47;
				b.y=35;
				b.w=5;
				b.h=9;
			break;
			case 4:
				b.x=58;
				b.y=36;
				b.w=5;
				b.h=5;
			break;
			case 5:
				b.x=733;
				b.y=0;
				b.w=7;
				b.h=7;
			break;
			case 6:
				b.x=741;
				b.y=0;
				b.w=3;
				b.h=8;
			break;
		}
		a.x=ft->x;
		a.y=ft->y;
		SDL_BlitSurface(gfx, &b, screen, &a);

		ftp=ft;
		ft=ft->n;
	}
}

void
engine_player(pDesc *p)
{
	SDL_Rect a,b;
	eDesc *e;

/* calc move */

if(p->cftime<p->ftime)
	p->cftime++;
else {
	p->cftime=0;
	if(p->incx) {
		if(p->incx>0) {
			if(p->x+4<SCREENW-40)
				p->x+=4;
			else {
				p->shield--;
				if(p->shield>0)
					engine_add_vefx(VFX_SHIELD,p->x,p->y);
				else
					engine_add_vefx(VFX_EXPLO,p->x,p->y);
			}
		} else {
			if(p->x-4>20)
				p->x-=4;
			else {
				p->shield--;
				if(p->shield>0)
					engine_add_vefx(VFX_SHIELD,p->x,p->y);
				else
					engine_add_vefx(VFX_EXPLO,p->x,p->y);
			}
		}
	}

	if(p->incy) {
		if(p->incy>0) {
			if(p->y+4<SCREENH-27)
				p->y+=4;
		} else {
			if(p->y-4>40)
				p->y-=4;
		}
	}

	/* enemy hit */
	for(e=eused; e; e=e->n) {
		if(e->shield>0) {
			/* just 4 point, player size hardcoded (!) */
			if((e->hit)(e,p->x+5,p->y+5) || (e->hit)(e,p->x+15,p->y+5) ||
				(e->hit)(e,p->x+15,p->y+15) || (e->hit)(e,p->x+5,p->y+15) ) {
					if(p->shield>0)
						engine_add_vefx(VFX_SHIELD,p->x,p->y);
					else
						engine_add_vefx(VFX_EXPLO,p->x,p->y);

					e->shield--;
					p->shield--;
					if(e->shield<=0)
						engine_add_vefx(VFX_EXPLO,e->x+6,e->y+6);
			}
			/* and companion */
			if(p->companion) {
				if((e->hit)(e,p->cx+3,p->cy+3)) {
						engine_add_vefx(VFX_EXPLOB,p->cx-10,p->cy-10);
						e->shield--;
						p->companion=0;
						if(e->shield<=0)
							engine_add_vefx(VFX_EXPLO,e->x+6,e->y+6);
				}
			}
		}
	}	

	/* fire */
	if(p->fire==1) {
		p->fire++;

		if(p->firef!=0)
			p->firef=0;
		else
			p->firef=1;		
		
		switch(p->weapon) {
			default:
			case 0:
				switch(p->level) {
					default:
					case 0:
						engine_add_fire(1,p->weapon,p->x+8,p->y,0,-1,p);
					break;
					case 1:
						if(p->firef)
							engine_add_fire(1,p->weapon,p->x+4,p->y,0,-1,p);
						else
							engine_add_fire(1,p->weapon,p->x+12,p->y,0,-1,p);
					break;
					case 2:
						if(p->firef)
							engine_add_fire(1,p->weapon,p->x+8,p->y,0,-1,p);
						engine_add_fire(1,p->weapon,p->x+4,p->y,-0.10,-1,p);
						engine_add_fire(1,p->weapon,p->x+12,p->y,0.10,-1,p);
					break;
					case 3:
						if(p->firef) {
							engine_add_fire(1,p->weapon,p->x+4,p->y,0,-1,p);
							engine_add_fire(1,p->weapon,p->x+12,p->y,0,-1,p);
						} else {
							engine_add_fire(1,p->weapon,p->x+4,p->y,-0.10,-1,p);
							engine_add_fire(1,p->weapon,p->x+12,p->y,0.10,-1,p);
						}
					break;
					case 4:
						if(p->firef) {
							engine_add_fire(1,p->weapon,p->x+8,p->y,0,-1,p);
							engine_add_fire(1,p->weapon,p->x+4,p->y,-0.20,-1,p);
							engine_add_fire(1,p->weapon,p->x+12,p->y,0.20,-1,p);
						} else {
							engine_add_fire(1,p->weapon,p->x+4,p->y,-0.10,-1,p);
							engine_add_fire(1,p->weapon,p->x+14,p->y,0.10,-1,p);
						}
					break;
				}
			break;
			case 1:
				switch(p->level) {
					default:
					case 0:
						engine_add_fire(1,p->weapon,p->x+8,p->y,0,-1,p);
					break;
					case 1:
						if(p->firef)
							engine_add_fire(1,p->weapon,p->x+8,p->y,0,-1,p);
						else {
							engine_add_fire(1,p->weapon,p->x+8,p->y,0,-1,p);
							engine_add_fire(1,p->weapon,p->x+4,p->y+8,-0.25,1,p);
							engine_add_fire(1,p->weapon,p->x+12,p->y+8,0.25,1,p);
						}
					break;
					case 2:
						if(p->firef) {
							engine_add_fire(1,p->weapon,p->x+8,p->y,0,-1,p);
							engine_add_fire(1,p->weapon,p->x+4,p->y+8,-0.25,1,p);
							engine_add_fire(1,p->weapon,p->x+12,p->y+8,0.25,1,p);
						} else {
							engine_add_fire(1,p->weapon,p->x+4,p->y,-0.10,-1,p);
							engine_add_fire(1,p->weapon,p->x+12,p->y,0.10,-1,p);
						}
					break;
					case 3:
						if(p->firef) {
							engine_add_fire(1,p->weapon,p->x+4,p->y,0,-1,p);
							engine_add_fire(1,p->weapon,p->x+12,p->y,0,-1,p);
							engine_add_fire(1,p->weapon,p->x+4,p->y+8,-0.25,1,p);
							engine_add_fire(1,p->weapon,p->x+12,p->y+8,0.25,1,p);
						} else {
							engine_add_fire(1,p->weapon,p->x+4,p->y,-0.10,-1,p);
							engine_add_fire(1,p->weapon,p->x+12,p->y,0.10,-1,p);
						}
					break;
					case 4:
						if(p->firef) {
							engine_add_fire(1,p->weapon,p->x+8,p->y,0,-1,p);
							engine_add_fire(1,p->weapon,p->x+4,p->y+8,-0.25,1,p);
							engine_add_fire(1,p->weapon,p->x+12,p->y+8,0.25,1,p);
							engine_add_fire(1,p->weapon,p->x+4,p->y,-0.20,-1,p);
							engine_add_fire(1,p->weapon,p->x+12,p->y,0.20,-1,p);
						} else {
							engine_add_fire(1,p->weapon,p->x+4,p->y,-0.10,-1,p);
							engine_add_fire(1,p->weapon,p->x+14,p->y,0.10,-1,p);
						}
					break;
				}
			break;
			case 2:
				switch(p->level) {
					default:
					case 0:
						engine_add_fire(1,p->weapon,p->x+8,p->y-10,0,-1,p);
					break;
					case 1:
						if(p->firef)
							engine_add_fire(1,p->weapon,p->x+4,p->y,-0.10,-1,p);
						else
							engine_add_fire(1,p->weapon,p->x+12,p->y,0.10,-1,p);
					break;
					case 2:
						if(p->firef) {
							engine_add_fire(1,p->weapon,p->x+4,p->y,-0.10,-1,p);
							engine_add_fire(1,p->weapon,p->x+12,p->y,0.10,-1,p);
						} else
							engine_add_fire(1,p->weapon,p->x+8,p->y-10,0,-1,p);
					break;
					case 3:
						if(p->firef) {
							engine_add_fire(1,p->weapon,p->x+4,p->y,-0.25,-1,p);
							engine_add_fire(1,p->weapon,p->x+12,p->y,0.25,-1,p);
						} else {
							engine_add_fire(1,p->weapon,p->x+4,p->y,-0.10,-1,p);
							engine_add_fire(1,p->weapon,p->x+12,p->y,0.10,-1,p);
						}
					break;
					case 4:
						if(p->firef) {
							engine_add_fire(1,p->weapon,p->x+4,p->y,-0.25,-1,p);
							engine_add_fire(1,p->weapon,p->x+12,p->y,0.25,-1,p);
							engine_add_fire(1,p->weapon,p->x+12,p->y,0.45,-1,p);
						} else {
							engine_add_fire(1,p->weapon,p->x+4,p->y,-0.45,-1,p);
							engine_add_fire(1,p->weapon,p->x+4,p->y,-0.10,-1,p);
							engine_add_fire(1,p->weapon,p->x+12,p->y,0.10,-1,p);
							engine_add_fire(1,p->weapon,p->x+8,p->y-10,0,-1,p);
						}
					break;
				}
			break;
		}
		
		if(p->companion) {
			engine_add_fire(1,3,p->cx+2,p->cy-8,0,-1,p);
		}
	}
	
	/* next frame */
	p->cf++;
	if(p->cf==p->nf)
		p->cf=0;
		
	if(p->companion) {
		if(p->incx && p->incx!=p->cinc)
			p->cinc=p->incx;

		p->t+=10*p->cinc;
	}
}

/* player sprite */
a.x=p->x;
a.y=p->y;

b.x=p->f[p->h_dir].x;
b.y=p->f[p->h_dir].y;
b.w=p->f[p->h_dir].w;
b.h=p->f[p->h_dir].h;
SDL_BlitSurface(gfx, &b, screen, &a);

/* companion */
if(p->companion) {
	a.x=p->cx;
	a.y=p->cy;

	b.x=162;
	b.y=30;
	b.w=7;
	b.h=7;
	SDL_BlitSurface(gfx, &b, screen, &a);
	
	if(p->cx<25 || p->cx>SCREENW-32) {
		engine_add_vefx(VFX_EXPLOB,p->cx-10,p->cy-10);
		p->companion=0;
	} else {
		circle_path(p->x+10,p->y+10,30,p->t,&p->cx,&p->cy);
		p->cx-=4;
		p->cy-=4;
	}
}

}

void
circle_path(int x, int y, int r, int t, float *rx, float *ry)
{
	*rx=floor((double)r*cos((double)t/100)+(double)x);
	*ry=floor((double)r*sin((double)t/100)+(double)y);
}

void
engine_enemy()
{
	eDesc *et,*etp;

	if(!boss)
	{
		/* check action */
		actCnt++;
		if(actIdx==act_count-1) {
			#define END_STR_1 "EVIL HAS BEEN VANQUISHED"
			#define END_STR_2 "THE END"
			SDL_Delay(3000);
			SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
			SDL_Flip(screen);SDL_Delay(3000);
			nSDL_DrawString(screen, font1, 160 - (nSDL_GetStringWidth(font1, END_STR_1) / 2), 82, END_STR_1);
			SDL_Flip(screen);SDL_Delay(5000);
			nSDL_DrawString(screen, font2, 160 - (nSDL_GetStringWidth(font1, END_STR_2) / 2), 94, END_STR_2);
			SDL_Flip(screen);SDL_Delay(8000);
			SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
			SDL_Flip(screen);SDL_Delay(3000);
			nSDL_DrawString(screen, font1, 10, 10, "Commander,");
			SDL_Flip(screen);SDL_Delay(2000);
			nSDL_DrawString(screen, font1, 10, 22, "A new fleet has emerged from hyperspace just\noutside of the Milky Way.");
			SDL_Flip(screen);SDL_Delay(4000);
			nSDL_DrawString(screen, font1, 10, 42, "We need you...");
			SDL_Flip(screen);SDL_Delay(3000);
			nSDL_DrawString(screen, font1, 10, 54, "...");
			SDL_Flip(screen);SDL_Delay(2000);
			nSDL_DrawString(screen, font1, 10, 66, "...");
			SDL_Flip(screen);SDL_Delay(3000);
			nSDL_DrawString(screen, font1, 10, 78, "Please respond.");
			SDL_Flip(screen);SDL_Delay(10000);
			done=true;
			return;
		}
		while(actCnt==act[actIdx].a){
			if(act[actIdx].type) {
				engine_add_enemy(act[actIdx].type, act[actIdx].x, act[actIdx].y);
				actIdx++;
				actCnt=0;
			} else {/* if the type == 0 then it's a vefx */
			       /* x value holds the kind and y holds type data */
				engine_add_vefx(act[actIdx].x, act[actIdx].x, act[actIdx].y);
				actIdx++;
				actCnt=0;
			}}

		/* temporal loop */
		if(actCnt>1000) {
			actCnt=0;
			actIdx=0;
		}
	}

	/* process all the enemies */
	for(etp=et=eused;et;) {
		if(et->cftime<et->ftime)
			et->cftime++;
		else {
			et->cftime=0;
			if(et->shield<=0) {

				if(et->type==4)
					engine_add_obj(OBJ_WEAPON1, et->x+20, et->y+20);

				if(et->type==6 || et->type==10)
				{
					boss=false;
					engine_add_obj(OBJ_SHIELD, et->x+46, et->y+26);
				}

				if(et->type==7 || et->type==9)
				{
					engine_add_fire(2,5,et->x+16,et->y+16,-1,0.5,NULL);
					engine_add_fire(2,5,et->x+16,et->y+16,1,-0.5,NULL);
					engine_add_fire(2,5,et->x+16,et->y+16,0.5,1,NULL);
					engine_add_fire(2,5,et->x+16,et->y+16,-0.5,-1,NULL);

					engine_add_fire(2,5,et->x+16,et->y+16,0.5,-1,NULL);
					engine_add_fire(2,5,et->x+16,et->y+16,-0.5,1,NULL);
					engine_add_fire(2,5,et->x+16,et->y+16,1,0.5,NULL);
					engine_add_fire(2,5,et->x+16,et->y+16,-1,-0.5,NULL);
				}
				
				if(et==eused) {
					etp=efree;
					efree=eused;
					eused=eused->n;
					efree->n=etp;
					etp=et=eused;
					continue;
				} else {
					etp->n=et->n;
					et->n=efree;
					efree=et;
					et=etp->n;
					continue;
				}
			}
			/* IA */
			(et->ia)(et);
		}

		/* DRAW */
		(et->draw)(et);

		etp=et;
		et=et->n;
	}
}

void
engine_add_enemy(int type, int x, int y)
{
	eDesc *et;

	if(efree==NULL) {
		fprintf(stderr,"PANIC!!!! efree reached limit\n");
		QUIT();
	}

	if(eused==NULL) {
		eused=efree;
		efree=efree->n;
		eused->n=NULL;
	} else {
		et=eused;
		eused=efree;
		efree=efree->n;
		eused->n=et;
	}

	eused->type=type;
	eused->x=x;
	eused->y=y;
	eused->cftime=0;
	eused->ftime=2;
	eused->init=0;
	memset(eused->var,0,sizeof(int)*10);

	switch(type) {
		default:
			fprintf(stderr,"FATAL: undefined enemy!\n");
			QUIT();
		break;
		case 1:
			eused->shield=2;
			eused->score=10;
			eused->ia=enemy_type1;
			eused->draw=enemy_type1d;
			eused->hit=enemy_type1h;
		break;
		case 2:
			eused->shield=1;
			eused->score=5;
			eused->var[4] = lfsr113() & 1;
			eused->ia=enemy_type2;
			eused->draw=enemy_type2d;
			eused->hit=enemy_type2h;
		break;
		case 3:
			eused->shield=1;
			eused->score=10;
			eused->var[4] = lfsr113() & 1;
			eused->ia=enemy_type3;
			eused->draw=enemy_type3d;
			eused->hit=enemy_type3h;
		break;
		case 4:
			eused->shield=20;
			eused->score=25;
			eused->ia=enemy_type4;
			eused->draw=enemy_type4d;
			eused->hit=enemy_type4h;
		break;
		case 5:
			eused->shield=2;
			eused->score=15;
			eused->ia=enemy_type5;
			eused->draw=enemy_type5d;
			eused->hit=enemy_type5h;
		break;
		case 6:
			eused->shield=350;
			eused->score=30;
			eused->ia=enemy_type6;
			eused->draw=enemy_type6d;
			eused->hit=enemy_type6h;
			boss=true;
		break;
		case 7:
			eused->shield=1;
			eused->score=25;
			eused->ia=enemy_type7;
			eused->draw=enemy_type7d;
			eused->hit=enemy_type7h;
		break;
		case 8:
			eused->shield=8;
			eused->score=5;
			eused->ia=enemy_type8;
			eused->draw=enemy_type8d;
			eused->hit=enemy_type8h;
		break;
		case 9:
			eused->shield=20;
			eused->score=20;
			eused->ia=enemy_type9;
			eused->draw=enemy_type9d;
			eused->hit=enemy_type9h;
		break;
		case 10:
			eused->shield=600;
			eused->score=40;
			eused->ia=enemy_type10;
			eused->draw=enemy_type10d;
			eused->hit=enemy_type10h;
			boss=true;
		break;
		case 11:
			eused->shield=1;
			eused->score=5;
			eused->var[4] = lfsr113() & 1;
			eused->ia=enemy_type11;
			eused->draw=enemy_type11d;
			eused->hit=enemy_type11h;
		break;
	}
}

/* ********************************************* */

void
enemy_type1(eDesc *e)
{
	int i;
	float m[2]={1.0f,1.0f};
	/*
	   var 0 -> radian
	   var 1 -> loop time

	   var 2,3 -> fire

	   var 4,5 -> frames (for fire)
	*/

	if(e->init<3) {
		if(e->var[3]==60) {
			e->var[2]=1;
			e->var[3]=20;
		} else
			e->var[3]++;

		if(e->var[2]) {
			for(i=0;i<2;i++)
				if(player[i].shield>0) {
					if(e->y<player[i].y) {
						if((player[i].y+10)-(e->y+28)!=0)
							m[i]=(double)((player[i].x+10)-(e->x+14))/(double)((player[i].y+10)-(e->y+28));
					} else {
						if((e->y-4)-(player[i].y+10)!=0)
							m[i]=(double)((player[i].x+10)-(e->x+14))/(double)((e->y-4)-(player[i].y+10));
					}
				}
			if(abs(m[0])<abs(m[1])) {
				i=0;
			} else
				i=1;

			if(abs(m[i])<0.3) {
				if(e->y<player[i].y)
					engine_add_fire(2,4,e->x+14,e->y+28,m[i],1,NULL);
				else
					engine_add_fire(2,4,e->x+14,e->y-4,m[i],-1,NULL);
				e->var[2]=0;
				e->var[4]=1;
				e->var[5]=8;
			}
		}
	}


	switch(e->init) {
		default:
			e->shield=0;
		break;
		case 0:
			e->y+=6;
			if(e->y==64) {
				e->init++;
				e->var[0]=325;
				e->var[1]=0;
			}
		break;
		case 1:
			circle_path(140,76,80,e->var[0],&e->x,&e->y);
			e->var[0]-=7;
			if(e->var[1]!=140)
				e->var[1]++;
			else
				e->init++;
		break;
		case 2:
			if(e->y>-32)
				e->y-=6;
			else
				e->init++;
		break;
	}

	if(e->var[5]) {
		e->var[5]--;
		if(!e->var[5]) {
			e->var[4]=0;
		}
	}
}

void
enemy_type1d(eDesc *e)
{
	SDL_Rect b[2]={{ 7,136,21,28 },{ 42,136,21,28 }};
	SDL_Rect a;

	a.x=e->x;
	a.y=e->y;

	SDL_BlitSurface(gfx, &b[e->var[4]], screen, &a);
}

int
enemy_type1h(eDesc *e, int x, int y)
{
	return (x>e->x && x<e->x+21 && y>e->y && y<e->y+28);
}

/* ********************************************* */

void
enemy_type2(eDesc *e)
{
  int i;
	/*
	 	4 -> sprite
		0 -> direction
		1 -> fire
	*/
	switch(e->init) {
		default:
			e->shield=0;
		break;
		case 0:
			e->y+=2;
			if(e->y>40) {
				e->init++;
				if(e->x>160)
					e->var[0]=0;
				else
					e->var[0]=1;
			}
		break;
		case 1:
			if(e->y>SCREENH)
				e->init++;
			else
				e->y+=2;

			if(e->var[0]) {
				if(e->x+4>SCREENW-50)
					e->var[0]=0;
				else
					e->x+=4;
			} else {
				if(e->x-4<30)
					e->var[0]=1;
				else
					e->x-=4;
			}

			for(i=0;!e->var[1] && i<2;i++)
				if(player[i].shield>0)
					if(abs(player[i].x-e->x)<8 && player[i].y>e->y) {
						engine_add_fire(2,4,e->x+10,e->y+20,0,+1.5,NULL);
						e->var[1]=1;
					}
		break;
	}
}

void
enemy_type2d(eDesc *e)
{
	SDL_Rect b[2]={{ 76,136,20,26 },{ 112,137,20,26 }};
	SDL_Rect a;

	a.x=e->x;
	a.y=e->y;

	SDL_BlitSurface(gfx, &b[e->var[4]], screen, &a);
}

int
enemy_type2h(eDesc *e, int x, int y)
{
	return (x>e->x && x<e->x+20 && y>e->y && y<e->y+26);
}

/* ********************************************* */

void
enemy_type3(eDesc *e)
{
  int i;
	/*
	 	4 -> frame control
		1 -> fire
	*/
	switch(e->init) {
		default:
			e->shield=0;
		break;
		case 0:
			if(e->y>SCREENH)
				e->init++;
			else
				e->y+=3;

			for(i=0;!e->var[1] && i<2;i++)
				if(player[i].shield>0)
					if(abs(player[i].x-e->x)<8 && player[i].y>e->y) {
						engine_add_fire(2,4,e->x+10,e->y+20,0,+1.5,NULL);
						e->var[1]=1;
					}

		break;
	}
}

/* ********************************************* */

void
enemy_type4(eDesc *e)
{
  int i;
  float m[2]={1.0f,1.0f};
  
	/*
		1 -> fire cycle
		2 -> timer
	 	4,5 -> frame control
	*/
	switch(e->init) {
		default:
			e->shield=0;
		break;
		case 0:
			e->y+=2;
			if(e->y>20) {
				e->init++;
				e->var[5]=2;
				e->var[1]++;
			}
		break;
		case 1:
			e->y+=0.5;
			e->var[2]++;
			if(e->var[2]>150)
				e->init++;
		break;
		case 2:
			e->y+=2;
			e->var[5]=0;
			e->var[1]=0;
		break;
	}

	if(e->var[1]>0) {
		switch(e->var[1]) {
			case 2:
				engine_add_fire(2,5,e->x+22,e->y+16,1,1,NULL);
				engine_add_fire(2,5,e->x+22,e->y+16,1,-1,NULL);
				engine_add_fire(2,5,e->x+22,e->y+16,-1,1,NULL);
				engine_add_fire(2,5,e->x+22,e->y+16,-1,-1,NULL);
				engine_add_vefx(VFX_EXPLOB,e->x+16,e->y+10);
			break;
			case 8:
				engine_add_fire(2,5,e->x+22,e->y+16,0.5,1,NULL);
				engine_add_fire(2,5,e->x+22,e->y+16,0.5,-1,NULL);
				engine_add_fire(2,5,e->x+22,e->y+16,-0.5,1,NULL);
				engine_add_fire(2,5,e->x+22,e->y+16,-0.5,-1,NULL);
				engine_add_vefx(VFX_EXPLOB,e->x+16,e->y+10);
			break;
			case 14:
				engine_add_fire(2,5,e->x+22,e->y+16,1,0.5,NULL);
				engine_add_fire(2,5,e->x+22,e->y+16,1,-0.5,NULL);
				engine_add_fire(2,5,e->x+22,e->y+16,-1,0.5,NULL);
				engine_add_fire(2,5,e->x+22,e->y+16,-1,-0.5,NULL);
				engine_add_vefx(VFX_EXPLOB,e->x+16,e->y+10);
			break;
			case 20:
				engine_add_fire(2,5,e->x+22,e->y+16,-1,0,NULL);
				engine_add_fire(2,5,e->x+22,e->y+16,1,0,NULL);
				engine_add_fire(2,5,e->x+22,e->y+16,0,1,NULL);
				engine_add_fire(2,5,e->x+22,e->y+16,0,-1,NULL);
				engine_add_vefx(VFX_EXPLOB,e->x+16,e->y+10);
			break;
			case 25:
			case 30:
			case 35:
				for(i=0;i<2;i++)
					if(player[i].shield>0) {
						if(e->y<player[i].y) {
							if((player[i].y+10)-(e->y+21)!=0)
								m[i]=(double)((player[i].x+10)-(e->x+21))/(double)((player[i].y+10)-(e->y+21));
						} else {
							if((e->y+21)-(player[i].y+10)!=0)
								m[i]=(double)((player[i].x+10)-(e->x+21))/(double)((e->y+21)-(player[i].y+10));
						}
					}
				if(abs(m[0])<abs(m[1])) {
					i=0;
				} else
					i=1;

				if(abs(m[i])<0.3) {
					if(e->y<player[i].y)
						engine_add_fire(2,4,e->x+22,e->y+39,m[i],1,NULL);
					else
						engine_add_fire(2,4,e->x+22,e->y+39,m[i],-1,NULL);
				}
			break;
		}
		if(e->var[1]>40)
			e->var[1]=1;
		else
			e->var[1]++;
	}
}

void
enemy_type4d(eDesc *e)
{
	SDL_Rect b={245, 79, 56, 52};
	SDL_Rect a;

	a.x=e->x;
	a.y=e->y;

	SDL_BlitSurface(gfx, &b, screen, &a);
}

int
enemy_type4h(eDesc *e, int x, int y)
{
	return (x>e->x && x<e->x+56 && y>e->y && y<e->y+52);
}

/* ********************************************* */

void
enemy_type5(eDesc *e)
{
	/*
		0 -> direction
		1 -> fire
	*/
	switch(e->init) {
		default:
			e->shield=0;
		break;
		case 0:
			e->init++;
			e->var[1]=(int)e & 0x0f; /* pseudo random :) */
		case 1:
			if(e->y>SCREENH)
				e->init++;
			else
				e->y+=4;

			if(e->var[0]) {
				if(e->x+5>SCREENW-50)
					e->var[0]=0;
				else
					e->x+=5;
			} else {
				if(e->x-5<30)
					e->var[0]=1;
				else
					e->x-=5;
			}

			if(e->var[1]>16) {
				engine_add_fire(2,6,e->x+8,e->y+20,0,+1.5,NULL);
				engine_add_fire(2,6,e->x+21,e->y+20,0,+1.5,NULL);
				e->var[1]=0;
			}
			e->var[1]++;
		break;
	}
}

void
enemy_type5d(eDesc *e)
{
	SDL_Rect b={ 147,138,22,25 };
	SDL_Rect a;

	a.x=e->x;
	a.y=e->y;

	SDL_BlitSurface(gfx, &b, screen, &a);
}

int
enemy_type5h(eDesc *e, int x, int y)
{
	return (x>e->x && x<e->x+22 && y>e->y && y<e->y+25);
}

/* ********************************************* */

void
enemy_type6(eDesc *e)
{
  int i;
  float m[2]={1.0f,1.0f};
  
	/*
		1 -> fire cycle
		2 -> timer
		3 -> fire type
	 	4 -> frame control
	*/
	switch(e->init) {
		default:
			e->shield=0;
		break;
		case 0:
			e->y+=2;
			if(e->y==20) {
				e->init++;
				e->var[1]=1;
			}
		break;
		case 1:
		break;
		case 2:
			e->y+=2;
			e->x-=2;
			if(e->x<25)
				e->init++;
		break;
		case 3:
			e->y-=2;
			e->x+=2;
			if(e->y==20)
				e->init++;
		break;
		case 4:
			e->y+=2;
			e->x+=2;
			if(e->x>223)
				e->init++;
		break;
		case 5:
			e->y-=2;
			e->x-=2;
			if(e->y==20)
			{
				e->init=1;
				e->var[1]=1;
				e->var[3]=0;
			}
		break;
	}

	if(e->var[1]>0) {
		if(e->var[3]==0)
			switch(e->var[1]) {
				case 2:
					engine_add_fire(2,5,e->x+17,e->y+26,1,1,NULL);
					engine_add_fire(2,5,e->x+17,e->y+26,1,-1,NULL);
					engine_add_fire(2,5,e->x+17,e->y+26,-1,1,NULL);
					engine_add_fire(2,5,e->x+17,e->y+26,-1,-1,NULL);
					engine_add_vefx(VFX_EXPLOB,e->x+8,e->y+24);

					engine_add_fire(2,5,e->x+17+49,e->y+26,1,1,NULL);
					engine_add_fire(2,5,e->x+17+49,e->y+26,1,-1,NULL);
					engine_add_fire(2,5,e->x+17+49,e->y+26,-1,1,NULL);
					engine_add_fire(2,5,e->x+17+49,e->y+26,-1,-1,NULL);
					engine_add_vefx(VFX_EXPLOB,e->x+8+49,e->y+24);
				break;
				case 8:
					engine_add_fire(2,5,e->x+17,e->y+26,0.5,1,NULL);
					engine_add_fire(2,5,e->x+17,e->y+26,0.5,-1,NULL);
					engine_add_fire(2,5,e->x+17,e->y+26,-0.5,1,NULL);
					engine_add_fire(2,5,e->x+17,e->y+26,-0.5,-1,NULL);
					engine_add_vefx(VFX_EXPLOB,e->x+8,e->y+24);

					engine_add_fire(2,5,e->x+17+49,e->y+26,0.5,1,NULL);
					engine_add_fire(2,5,e->x+17+49,e->y+26,0.5,-1,NULL);
					engine_add_fire(2,5,e->x+17+49,e->y+26,-0.5,1,NULL);
					engine_add_fire(2,5,e->x+17+49,e->y+26,-0.5,-1,NULL);
					engine_add_vefx(VFX_EXPLOB,e->x+8+49,e->y+24);
				break;
				case 14:
					engine_add_fire(2,5,e->x+17,e->y+26,1,0.5,NULL);
					engine_add_fire(2,5,e->x+17,e->y+26,1,-0.5,NULL);
					engine_add_fire(2,5,e->x+17,e->y+26,-1,0.5,NULL);
					engine_add_fire(2,5,e->x+17,e->y+26,-1,-0.5,NULL);
					engine_add_vefx(VFX_EXPLOB,e->x+8,e->y+24);
					
					engine_add_fire(2,5,e->x+17+49,e->y+26,1,0.5,NULL);
					engine_add_fire(2,5,e->x+17+49,e->y+26,1,-0.5,NULL);
					engine_add_fire(2,5,e->x+17+49,e->y+26,-1,0.5,NULL);
					engine_add_fire(2,5,e->x+17+49,e->y+26,-1,-0.5,NULL);
					engine_add_vefx(VFX_EXPLOB,e->x+8+49,e->y+24);
				break;
				case 20:
					engine_add_fire(2,5,e->x+17,e->y+26,-1,0,NULL);
					engine_add_fire(2,5,e->x+17,e->y+26,1,0,NULL);
					engine_add_fire(2,5,e->x+17,e->y+26,0,1,NULL);
					engine_add_fire(2,5,e->x+17,e->y+26,0,-1,NULL);
					engine_add_vefx(VFX_EXPLOB,e->x+8,e->y+24);

					engine_add_fire(2,5,e->x+17+49,e->y+26,-1,0,NULL);
					engine_add_fire(2,5,e->x+17+49,e->y+26,1,0,NULL);
					engine_add_fire(2,5,e->x+17+49,e->y+26,0,1,NULL);
					engine_add_fire(2,5,e->x+17+49,e->y+26,0,-1,NULL);
					engine_add_vefx(VFX_EXPLOB,e->x+8+49,e->y+24);
				break;
				case 25:
				case 30:
				case 35:
					for(i=0;i<2;i++)
						if(player[i].shield>0) {
							if(e->y<player[i].y) {
								if((player[i].y+10)-(e->y+21)!=0)
									m[i]=(double)((player[i].x+10)-(e->x+21))/(double)((player[i].y+10)-(e->y+21));
							} else {
								if((e->y+21)-(player[i].y+10)!=0)
									m[i]=(double)((player[i].x+10)-(e->x+21))/(double)((e->y+21)-(player[i].y+10));
							}
						}
					if(abs(m[0])<abs(m[1])) {
						i=0;
					} else
						i=1;

					if(abs(m[i])<0.3) {
						if(e->y<player[i].y)
							engine_add_fire(2,4,e->x+35,e->y+38,m[i],1,NULL);
						else
							engine_add_fire(2,4,e->x+35,e->y+38,m[i],-1,NULL);
					}
				break;
			}
		else
		{

			switch(e->var[1]) {
				case 2:
				case 14:
					engine_add_fire(2,6,e->x+20,e->y+40,0,+1.5,NULL);
					engine_add_fire(2,6,e->x+20+27,e->y+40,0,+1.5,NULL);
				break;
				case 35:
					for(i=0;i<2;i++)
						if(player[i].shield>0) {
							if(e->y<player[i].y) {
								if((player[i].y+10)-(e->y+21)!=0)
									m[i]=(double)((player[i].x+10)-(e->x+21))/(double)((player[i].y+10)-(e->y+21));
							} else {
								if((e->y+21)-(player[i].y+10)!=0)
									m[i]=(double)((player[i].x+10)-(e->x+21))/(double)((e->y+21)-(player[i].y+10));
							}
						}
					if(abs(m[0])<abs(m[1])) {
						i=0;
					} else
						i=1;

					if(abs(m[i])<0.3) {
						if(e->y<player[i].y)
							engine_add_fire(2,4,e->x+35,e->y+38,m[i],1,NULL);
						else
							engine_add_fire(2,4,e->x+35,e->y+38,m[i],-1,NULL);
					}
				break;
				default:
				break;
			}
		}

		if(e->var[1]>40)
		{
			e->var[1]=1;
			if(e->var[3]==0)
			{
				e->init++;
				e->var[3]=1;
			}
		}
		else
			e->var[1]++;
	}
	
	e->var[4]++;
	if(e->var[4]>3)
		e->var[4]=0;

}

void
enemy_type6d(eDesc *e)
{
	SDL_Rect b[4]={{ 664,40,85,53 },{ 664,94,85,53 },{ 664,149,85,53 },{ 664,94,85,53 }};
	SDL_Rect a;

	a.x=e->x;
	a.y=e->y;

	SDL_BlitSurface(gfx, &b[e->var[4]], screen, &a);
}

int
enemy_type6h(eDesc *e, int x, int y)
{
	return (x>e->x && x<e->x+85 && y>e->y && y<e->y+53);
}

/* ********************************************* */

void
enemy_type7(eDesc *e)
{
	/*
		1 -> timer
	 	4 -> frame control
	*/
	switch(e->init) {
		default:
			e->shield=0;
		break;
		case 0:
			e->y++;
			if(e->y>SCREENH)
				e->init++;
		break;
	}

	e->var[1]++;
	if(e->var[1]==2)
	{
		e->var[1]=0;
		e->var[4]++;
		if(e->var[4]>3)
			e->var[4]=0;
	}

}

void
enemy_type7d(eDesc *e)
{
	SDL_Rect b[4]={{ 211,134,32,32 },{ 246,134,32,32 },{ 211,169,32,32 },{ 246,169,32,32 }};
	SDL_Rect a;

	a.x=e->x;
	a.y=e->y;

	SDL_BlitSurface(gfx, &b[e->var[4]], screen, &a);
}

/* ********************************************* */

void
enemy_type8(eDesc *e)
{
	int k;

	/*
	 	0 -> timer
		5 -> fire timer
	*/
	switch(e->init) {
		default:
			e->shield=0;
		break;
		case 0:
			e->y+=4;
			if(e->y>20)
			{
				e->var[0]=80;
				e->init++;
			}
		break;
		case 1:
			e->var[5]--;

			k=-1;
			if(player[0].shield>0)
				k=0;
				
			if(player[1].shield>0)
			{
				if(k==-1)
					k=1;
				else
					if(abs(player[1].x-e->x)>abs(player[0].x-e->x))
						k=1;
			}

			if(k!=-1)
			{

				if(abs(e->x-player[k].x)<16)
				{
					if(e->var[5]<0)
					{
						engine_add_fire(2,6,e->x+5,e->y+23,0,2,NULL);
						engine_add_fire(2,6,e->x+27,e->y+23,0,2,NULL);
						e->var[5]=6;
					}
				}


				if(abs(e->x-player[k].x)>8)
				{
					if(e->x>player[k].x)
						e->var[1]=-2;
					else
						e->var[1]=+2;

					if(e->x+e->var[1]<SCREENW-55 && e->x+e->var[1]>30)
						e->x+=e->var[1];
				}

				e->var[0]--;
				if(e->var[0]==0)
					e->init++;
			}
			else
				e->init++;
		break;
		case 2:
			e->var[5]--;

			if(e->y>SCREENH)
			{
				e->init++;
				break;
			}
			else
				e->y+=2;

			if(e->var[1]>0) {
				if(e->x+4>SCREENW-55)
					e->var[1]=0;
				else
					e->x+=4;
			} else {
				if(e->x-4<30)
					e->var[1]=1;
				else
					e->x-=4;
			}

			k=0;
			if(player[0].shield>0)
				k=(int)(abs(e->x-player[0].x)<16);
			if(player[1].shield>0)
				k+=(int)(abs(e->x-player[0].x)<16);
			if(k>0)
			{
				if(e->var[5]<0)
				{
					engine_add_fire(2,6,e->x+5,e->y+23,0,2,NULL);
					engine_add_fire(2,6,e->x+27,e->y+23,0,2,NULL);
					e->var[5]=6;
				}
			}
		break;

	}
}

void
enemy_type8d(eDesc *e)
{
	SDL_Rect b={ 141,169,32,32 };
	SDL_Rect a;

	a.x=e->x;
	a.y=e->y;

	SDL_BlitSurface(gfx, &b, screen, &a);
}


/* ********************************************* */

void
enemy_type9(eDesc *e)
{
	/*
	 	0 -> status
		1 -> timer
	 	4 -> frame control
	*/
	switch(e->init) {
		default:
			e->shield=0;
		break;
		case 0:
			e->y+=2;
			if(e->y>40)
				e->init++;
		break;
		case 1:
			e->var[1]++;
			if(e->var[1]>10)
			{
				engine_add_vefx(VFX_EXPLOB,e->x,e->y);
				engine_add_enemy(7, e->x-8, e->y-8);
				
				engine_add_vefx(VFX_EXPLOB,e->x+32,e->y);
				engine_add_enemy(7, e->x+32, e->y-8);
				
				engine_add_vefx(VFX_EXPLOB,e->x-2,e->y+41);
				engine_add_enemy(7, e->x-8, e->y+28);

				engine_add_vefx(VFX_EXPLOB,e->x+32,e->y+41);
				engine_add_enemy(7, e->x+32, e->y+28);
				e->init++;
				e->var[0]=2;
			}
		break;
		case 2:
			e->y-=2;
			if(e->y<-41)
				e->init++;
		break;
	}

	e->var[4]=e->var[4] ? 0 : 1;
}

void
enemy_type9d(eDesc *e)
{
	SDL_Rect b[4]={{ 453,251,42,36 },{ 504,251,42,36 },{ 562,258,28,28 },{ 613,258,28,28 }};
	SDL_Rect a;

	a.x=e->x;
	a.y=e->y;

	SDL_BlitSurface(gfx, &b[e->var[0]+e->var[4]], screen, &a);
}

int
enemy_type9h(eDesc *e, int x, int y)
{
	if(e->var[0]==0)
		return (x>e->x && x<e->x+42 && y>e->y && y<e->y+36);
	else
		return (x>e->x && x<e->x+28 && y>e->y && y<e->y+28);
}

/* ********************************************* */

void
enemy_type10(eDesc *e)
{
  int i;
  float m[2]={1.0f,1.0f};

	/*
	 	0 -> status
		1 -> timer 
		2 -> dir
		3 -> timer
	 	4 -> frame control
	*/
	switch(e->init) {
		default:
			e->shield=0;
		break;
		case 0:
			e->y+=2;
			if(e->y==30) {
				e->init++;
				e->var[1]=60;
				e->var[3]=0;
			}
		break;
		case 1:
			e->var[3]++;

			e->var[1]--;
			if(e->var[1]==0)
			{
				e->init++;
				e->var[1]=150;
				e->var[3]=0;
			}
			else
			{
				if(e->var[3]==10)
				{
					e->var[3]=0;
					if(e->var[0]==0)
					{
						engine_add_fire(2,5,e->x+26,e->y+32,-1,0.5,NULL);
						engine_add_fire(2,5,e->x+26,e->y+32,1,-0.5,NULL);
						engine_add_fire(2,5,e->x+26,e->y+32,0.5,1,NULL);
						engine_add_fire(2,5,e->x+26,e->y+32,-0.5,-1,NULL);

						engine_add_fire(2,5,e->x+26,e->y+32,0.5,-1,NULL);
						engine_add_fire(2,5,e->x+26,e->y+32,-0.5,1,NULL);
						engine_add_fire(2,5,e->x+26,e->y+32,1,0.5,NULL);
						engine_add_fire(2,5,e->x+26,e->y+32,-1,-0.5,NULL);

						engine_add_vefx(VFX_EXPLOB,e->x+19,e->y+25);
					}
					if(e->var[0]<2)
					{
						engine_add_fire(2,5,e->x+6+91,e->y+32,-1,0.5,NULL);
						engine_add_fire(2,5,e->x+6+91,e->y+32,1,-0.5,NULL);
						engine_add_fire(2,5,e->x+6+91,e->y+32,0.5,1,NULL);
						engine_add_fire(2,5,e->x+6+91,e->y+32,-0.5,-1,NULL);

						engine_add_fire(2,5,e->x+6+91,e->y+32,0.5,-1,NULL);
						engine_add_fire(2,5,e->x+6+91,e->y+32,-0.5,1,NULL);
						engine_add_fire(2,5,e->x+6+91,e->y+32,1,0.5,NULL);
						engine_add_fire(2,5,e->x+6+91,e->y+32,-1,-0.5,NULL);

						engine_add_vefx(VFX_EXPLOB,e->x+90,e->y+25);
					}

					for(i=0;i<2;i++)
						if(player[i].shield>0) {
							if(e->y<player[i].y) {
								if((player[i].y+10)-(e->y+21)!=0)
									m[i]=(double)((player[i].x+10)-(e->x+52))/(double)((player[i].y+10)-(e->y+24));
							} else {
								if((e->y+21)-(player[i].y+10)!=0)
									m[i]=(double)((player[i].x+10)-(e->x+52))/(double)((e->y+24)-(player[i].y+10));
							}
						}
					if(abs(m[0])<abs(m[1])) {
						i=0;
					} else
						i=1;

					if(abs(m[i])<0.3) {
						if(e->y<player[i].y)
							engine_add_fire(2,4,e->x+52,e->y+24,m[i],1,NULL);
						else
							engine_add_fire(2,4,e->x+52,e->y+24,m[i],-1,NULL);
					}
				}
			}
		break;
		case 2:
			e->var[1]--;
			e->var[3]++;
			
			if(e->var[1]==0)
			{
				e->var[2]=0;
				if(e->var[0]!=2)
					e->init++;
			}
			else
			{
				if(e->var[2]==0)
				{
					if(e->var[0]!=2)
						e->x-=2;
					else
						e->x-=4;
					if(e->x<30)
						e->var[2]=1;
				}
				else
				{
					if(e->var[0]!=2)
						e->x+=2;
					else
						e->x+=4;
					if(e->x>SCREENW-30-110)
						e->var[2]=0;
				}

				if(e->var[3]==18)
				{
					e->var[3]=0;
					if(e->var[0]==0)
					{
						engine_add_fire(2,6,e->x+13,e->y+37,0,+1.5,NULL);
					}
					if(e->var[0]<2)
					{
						engine_add_fire(2,6,e->x+90,e->y+37,0,+1.5,NULL);
					}
				}

				if(e->var[3]==0 || e->var[3]==10)
				{
					for(i=0;i<2;i++)
						if(player[i].shield>0) {
							if(e->y<player[i].y) {
								if((player[i].y+10)-(e->y+21)!=0)
									m[i]=(double)((player[i].x+10)-(e->x+52))/(double)((player[i].y+10)-(e->y+24));
							} else {
								if((e->y+21)-(player[i].y+10)!=0)
									m[i]=(double)((player[i].x+10)-(e->x+52))/(double)((e->y+24)-(player[i].y+10));
							}
						}
					if(abs(m[0])<abs(m[1])) {
						i=0;
					} else
						i=1;

					if(abs(m[i])<0.3) {
						if(e->y<player[i].y)
							engine_add_fire(2,4,e->x+52,e->y+24,m[i],1,NULL);
						else
							engine_add_fire(2,4,e->x+52,e->y+24,m[i],-1,NULL);
					}
				}
			}
		break;
		case 3:
			if(e->var[2]==0)
			{
				e->y+=2;
				if(e->y>140)
					e->var[2]=1;
			}
			else
			{
				e->y-=2;
				if(e->y==30)
				{
					if(e->var[0]!=2)
						e->init=1;
					else
						e->init=2;
					e->var[1]=60;
					e->var[3]=0;
				}
			}
		break;
	}
}

void
enemy_type10d(eDesc *e)
{
	SDL_Rect b[2]={{ 664,204,65,75 },{ 729,204,63,75 }};
	SDL_Rect c[2]={{ 700,313,36,75 },{ 736,313,35,75 }};
	SDL_Rect a;

	if(e->shield>100)
	{
		a.x=e->x;
		a.y=e->y;

		SDL_BlitSurface(gfx, &b[0], screen, &a);
	}
	else
	{
		a.x=e->x+29;
		a.y=e->y;

		SDL_BlitSurface(gfx, &c[0], screen, &a);
	
		if(e->var[0]==0)
		{
			engine_add_vefx(VFX_MEXPLO,e->x,e->y+20);
			e->var[0]=1;
		}
	}

	if(e->shield>75)
	{
		a.x=e->x+65;
		a.y=e->y;

		SDL_BlitSurface(gfx, &b[1], screen, &a);
	}
	else
	{
		a.x=e->x+65;
		a.y=e->y;

		SDL_BlitSurface(gfx, &c[1], screen, &a);

		if(e->var[0]==1)
		{
			engine_add_vefx(VFX_MEXPLO,e->x+90,e->y+20);
			e->var[0]=2;
		}
	}
}

int
enemy_type10h(eDesc *e, int x, int y)
{
	switch(e->var[0])
	{
		default:
			return (x>e->x && x<e->x+128 && y>e->y && y<e->y+75);
		break;
		case 1:
			return (x>e->x+29 && x<e->x+128 && y>e->y && y<e->y+75);
		break;
		case 2:
			return (x>e->x+29 && x<e->x+100 && y>e->y && y<e->y+75);
		break;
	}
}

/* ********************************************* */

void
enemy_type11(eDesc *e)
{
  int i;
	/*
	 	4 -> frame control
		2 -> frame mod
		0 -> direction
		1 -> fire
		3 -> direction y
	*/
	switch(e->init) {
		default:
			e->shield=0;
		break;
		case 0:
			e->var[3]=-2;
			e->init++;
		break;
		case 1:
			if(e->var[2] && e->y>SCREENH)
				e->init++;
			else
				e->y+=e->var[3];

			if(e->y<-32)
				e->init++;
			
			if(e->var[0]) {
				if(e->x+4>SCREENW-50)
					e->var[0]=0;
				else
					e->x+=4;
			} else {
				if(e->x-4<30)
					e->var[0]=1;
				else
					e->x-=4;
			}

			if(e->var[2])
				for(i=0;!e->var[1] && i<2;i++)
					if(player[i].shield>0)
						if(abs(player[i].x-e->x)<8 && player[i].y>e->y) {
							engine_add_fire(2,4,e->x+10,e->y+20,0,+1.5,NULL);
							e->var[1]=1;
						}
		break;
		case 2:
			e->var[2]=2;
			e->var[3]=3;
			e->init=1;
		break;
	}
}

void
enemy_type11d(eDesc *e)
{
	SDL_Rect b[4]={{ 77,172,20,26 },{ 112,172,20,26 },{ 76,136,20,26 },{ 112,137,20,26 }};
	SDL_Rect a;

	a.x=e->x;
	a.y=e->y;

	SDL_BlitSurface(gfx, &b[e->var[4]+e->var[2]], screen, &a);
}

