#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

void drawUISignifiers(Adafruit_GFX &display, char leftChar, char rightChar, char okChar);
void displayPrintCenter(Adafruit_GFX &display, String text, int yValue, bool autoCenter);
void drawArrows(Adafruit_GFX &display);
void drawPlusMinus(Adafruit_GFX &display);
void drawTitle(Adafruit_GFX &display, char *text);