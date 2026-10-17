#include <Adafruit_SH110X.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

void drawUISignifiers(Adafruit_SH1107 &display, char leftChar, char rightChar, char okChar);
void displayPrintCenter(Adafruit_SH1107 &display, String text, int yValue, bool autoCenter);
void drawArrows(Adafruit_SH1107 &display);
void drawPlusMinus(Adafruit_SH1107 &display);
void drawTitle(Adafruit_SH1107 &display, char *text);