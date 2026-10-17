#include <Arduino.h>
#include "display.h"

void displayPrintCenter(Adafruit_GFX &display, String text, int yValue, bool autoCenter) {
  short int textX;
  short int textY;
  unsigned short int textWidth;
  unsigned short int textHeight;

  display.getTextBounds(text, 0, 0, &textX, &textY, &textWidth, &textHeight);

  float leftPadding = (SCREEN_WIDTH - textWidth) / 2.0;
  float upperPadding = textHeight / 2.0;

  if (autoCenter) {
    display.setCursor(leftPadding, yValue - upperPadding);
  } else {
    display.setCursor(leftPadding, yValue);
  }
  display.print(text);
}

void drawTitle(Adafruit_GFX &display, char *text) {
  display.setTextSize(1);
  display.setTextColor(1);

  displayPrintCenter(display, text, 6, true);
}

void drawUISignifiers(Adafruit_GFX &display, char leftChar, char rightChar, char okChar) {
  display.setTextSize(1);
  display.setCursor(0, (SCREEN_HEIGHT / 2) - 5);
  display.print(leftChar);

  display.setCursor(120, (SCREEN_HEIGHT / 2) - 5);
  display.print(rightChar);

  char okStr[2];
  sprintf(okStr, "%c", okChar);
  displayPrintCenter(display, okStr, SCREEN_HEIGHT - 6, true);
}

void drawArrows(Adafruit_GFX &display) {
  drawUISignifiers(display, '<', '>', 'V');
}

void drawPlusMinus(Adafruit_GFX &display) {
  drawUISignifiers(display, '-', '+', 'V');
}

