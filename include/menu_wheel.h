#ifndef MENUWHEEL
#define MENUWHEEL
#include <string>
#include "menu.h"
#include "buffer.h"
#define WHEELANGLE 6
#define WHEELVIEW 9
#define WHEELVIEWCHAR 7
#define SONGSPRITES 3
class Menu;
class MenuWheel {
private:
	Menu* menu;
	const int tilesYOffset[5] = {-1, 3, 9, 13, 15};
	bufferitem wheelitems[WHEELVIEW];
	u16* songFontGfx[SONGSPRITES * WHEELVIEWCHAR];
	u8 songFontSprite[SONGSPRITES * WHEELVIEWCHAR];
	u8 songFrameColor[WHEELVIEW];
	int tilesLen[5];
	int tilesTotalLen = 0;
	int bg1;
	int bg2;
	int anim = 0;
	void loadSongFontGfx();
	void loadFrameBg();
	void prev();
	void next();
	void renderChar(int angle);
	void updateColor();
	void rebuildBuffer(std::string path, std::string focus);
	bool upDirectory();
	bool colorSwap = false;
public:
	MenuWheel(Menu* menu);
	~MenuWheel();
	Buffer* buffer;
	void playAnim(int anim);
	void rebuildSongGfx();
	void updateFrameBg();
	void updateSong();
	void input();
	void render();
	int frame = 0;
};
#endif
