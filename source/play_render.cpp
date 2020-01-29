#include <nds.h>
#include <iostream>
#include <vector>
#include <tap.h>
#include <tail.h>
#include <hold.h>
#include "parse.h"
#include "play.h"
#include "play_render.h"

bool sprites[128];

using namespace std;

u16* tapMemory;
u16* tailMemory;

void pr_setup() {
	for (int i = 0; i < 128; i++) {
		pushSprite(i);
	}
	oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);
	tapMemory = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
	tailMemory = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
	dmaCopy(tapTiles, tapMemory, 32 * 32);
	dmaCopy(tailTiles, tailMemory, 32 * 32);
	dmaCopy(tapPal, SPRITE_PALETTE, 512);
	setRotData();
}

void renderSteps(vector<step> steps) {
	for (auto i = steps.begin(); i != steps.end(); i++) {
		if (i->y < 160) {
			switch (i->type) {
				case (1):
				case (2):
					oamSet(&oamMain, i->sprite, i->x, i->y, 0, 0, SpriteSize_32x32, SpriteColorFormat_16Color, tapMemory, i->col, false, false, false, false, false);
					break;
				case (3):
					oamSet(&oamMain, i->sprite, i->x, i->y, 0, 0, SpriteSize_32x32, SpriteColorFormat_16Color, tailMemory, 1, false, false, false, false, false);
					break;
			}
		} else {
			oamClearSprite(&oamMain, i->sprite);
		}
	}
}

void renderHolds(vector<hold> holds) {
	//sprites[127].attr0 = 100 | ATTR0_ROTSCALE_DOUBLE;
	//sprites[127].attr1 = 50 | ATTR1_SIZE_32 | ATTR1_ROTDATA(2);
	//sprites[127].attr2 = tiles_hold + (pal_hold << 12);
}

u8 popSprite() {
	for (u8 i = 0; i < 128; i++) {
		if (sprites[i]) {
			sprites[i] = FALSE;
			return i;
		}
	}
	sassert(0, "out of sprites");
	return 0;
}

void pushSprite(u8 i) {
	sprites[i] = TRUE;
	oamClearSprite(&oamMain, i);
}

void setRotData() {
	oamRotateScale(&oamMain, 0, degreesToAngle(90), intToFixed(1, 8), intToFixed(1, 8));
	oamRotateScale(&oamMain, 1, degreesToAngle(0), intToFixed(1, 8), intToFixed(1, 8));
	oamRotateScale(&oamMain, 2, degreesToAngle(180), intToFixed(1, 8), intToFixed(1, 8));
	oamRotateScale(&oamMain, 3, degreesToAngle(270), intToFixed(1, 8), intToFixed(1, 8));
}