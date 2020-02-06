#define HITYOFFSET 20

extern u16* tapMemory;
extern u16* tailMemory;
extern u16* holdMemory;

void pr_setup();
void renderPlay();
void renderSteps();
void renderCombo();
u8 popSprite();
void pushSprite(u8 i);
void setRotData();
