#define ASCIIOFFSET 32
#define CHARWIDTH 10
#define CHAROFFSET 3
#define CHARSPRITES 3
#define WHEELANGLE 6
typedef struct wheelitem{
	int type = -1;
	std::string name;
	std::string path;
	std::string smpath;
} wheelitem;
void mw_setup();
void loadSongFontGfx();
void loadFrameBg();
void wheelNext();
void wheelPrev();
void printToBitmap(u8 gfx, std::string str);
void fillWheel();
void fillWheelEmpty();
void updateWheelColor();
int indexToFile(int i);
int nearWheelCursor(int i);
bool parseDir(std::string dir, int index, int dest);
void renderWheel();
void playWheelAnim();
void renderWheelChar();
void updateFrameBg();