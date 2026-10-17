#include <Arduino.h>

#include <Adafruit_GFX.h>
#include <Wire.h>
#include <Adafruit_SH110X.h>

#include "images.h"
#include "display/display.h"

#define LEFT_BTN_PIN D10
#define RIGHT_BTN_PIN D9
#define SELECT_BTN_PIN D11

#define SMALL_CHARS_PER_LINE 18
#define LARGE_CHARS_PER_LINE 8
#define MAX_NUMBER_LINES 4

Adafruit_SH1107 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

enum Context {
  MAIN_SCREEN,
  DICE_SELECT,
  ROLL_NUMBER,
};

enum Presets {
  D4,
  D6,
  D8,
  D10,
  D12,
  D20,
  D100,
  SALU,
  NumberOfPresets,
};

char *PRESET_NAMES[] = {
  "D4", "D6", "D8", "D10", "D12", "D20", "D100", "Salu"
};

int CURRENT_SALU = -1;

// STATE VARIABLES

int CURRENT_HIGHLIGHTED_PRESET = D20;
int CURRENT_CONTEXT = MAIN_SCREEN;

int CURRENT_DICE_SIDES = 1;
int CURRENT_DICE_AMOUNT = 1;

bool CURRENTLY_ROLLING = false;

int RANDOM_NUMBERS[100];

const int RButPin = 35;
const int LButPin = 33;
const int SelButPin = 32;
const int TempButPin = 25;

const int DEBOUNCE_TIME = 250;

int lastRoll = millis();
int lastL = millis();
int lastR = millis();
int lastS = millis();
int lastT = millis();

void displayCurrentContext();
void handleLeftButtonPress();
void handleRightButtonPress();
void handleSelectPress();
void displayMainScreen();
void displaySaluScreen();
void displayDiceSelect(int preset);
void displayRollScreen();
void displayDigits();
void selectNextPreset();
void selectLastPreset();
void printNumberBuffer();
void displayCurrentRollAnimation();
void handleShaken();

void simpleRoll(int numberOfSides);
void simpleRollAnimation(int numberOfSides);

void createStatString(char* strBuffer);
bool currentNumbersFitLarge();
int numberOfDigits(int number);
float delayBlend(float x);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  if(!display.begin(0x3C, true)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  Serial.println("Screen initialized.");

  RANDOM_NUMBERS[0] = -1;

  display.clearDisplay();

  display.setTextColor(SH110X_WHITE);
  displayCurrentContext();
  pinMode(RButPin, INPUT);
  pinMode(LButPin, INPUT);
  pinMode(SelButPin, INPUT);
  pinMode(TempButPin, INPUT);
}

void displayCurrentContext() {
  Serial.print("CURRENT CONTEXT: ");
  Serial.println(CURRENT_CONTEXT);
  switch (CURRENT_CONTEXT) {
    case MAIN_SCREEN:
      displayMainScreen();
      break;
    case DICE_SELECT:
      displayDiceSelect(CURRENT_HIGHLIGHTED_PRESET);
      break;
    case ROLL_NUMBER:
      displayRollScreen();
      break;
    case SALU:
      displaySaluScreen();
      break;
  }
  /*
  Serial.print("Displaying context ");
  Serial.println(CURRENT_CONTEXT);
  */
}

void handleLeftButtonPress() {
  switch (CURRENT_CONTEXT) {
    case MAIN_SCREEN:
      selectLastPreset();
      displayCurrentContext();
      break;
    case DICE_SELECT:
      if (CURRENT_DICE_AMOUNT > 1) {
        CURRENT_DICE_AMOUNT -= 1;
        displayCurrentContext();
      }
      break;
  }
}

void handleRightButtonPress() {
  switch (CURRENT_CONTEXT) {
    case MAIN_SCREEN:
      selectNextPreset();
      displayCurrentContext();
      break;
    case DICE_SELECT:
      if (CURRENT_DICE_AMOUNT < 10) {
        CURRENT_DICE_AMOUNT += 1;
        displayCurrentContext();
      }
  }
}

void handleSelectPress() {
  switch (CURRENT_CONTEXT) {
    case MAIN_SCREEN:
      Serial.println(CURRENT_HIGHLIGHTED_PRESET);
      if (CURRENT_HIGHLIGHTED_PRESET == D6 || CURRENT_HIGHLIGHTED_PRESET == D10 || 
        CURRENT_HIGHLIGHTED_PRESET == D20 || CURRENT_HIGHLIGHTED_PRESET == D100 ||
        CURRENT_HIGHLIGHTED_PRESET == D8 || CURRENT_HIGHLIGHTED_PRESET == D12) {
        CURRENT_CONTEXT = DICE_SELECT;
        CURRENT_DICE_AMOUNT = 1;
        displayCurrentContext();
      } else if (CURRENT_HIGHLIGHTED_PRESET == SALU) {
        CURRENT_CONTEXT = SALU;
        displayCurrentContext();
      }
      break;
    case DICE_SELECT:
      CURRENT_CONTEXT = ROLL_NUMBER;
      displayCurrentContext();
      break;
    case SALU:
      CURRENT_CONTEXT = MAIN_SCREEN;
      CURRENT_SALU = -1;
      displayCurrentContext();
      break;
    case ROLL_NUMBER:
      int ticker = 0; 
      while (RANDOM_NUMBERS[ticker] != -1){
        RANDOM_NUMBERS[ticker]=-1;
        ticker +=1; 
      } 
      CURRENT_CONTEXT = MAIN_SCREEN;
      displayCurrentContext();
      break;
  } 

}

void displayMainScreen() {
  display.setTextSize(1);
  display.clearDisplay();
  drawArrows(display);
  drawTitle(display, "PRESET");

  display.setTextSize(2);
  displayPrintCenter(display, PRESET_NAMES[CURRENT_HIGHLIGHTED_PRESET], SCREEN_HEIGHT / 2, true);
  display.display();
}

void displaySaluScreen() {
  display.clearDisplay();
  if (CURRENT_SALU == -1) {
    display.setTextSize(1);
    displayPrintCenter(display, "Shake For Salu!", SCREEN_HEIGHT / 2, true);
  } else {
    if (CURRENT_SALU == 0) {
      display.drawBitmap(0, 0, epd_bitmap_salu_small, 128, 128, 1);
    } else if (CURRENT_SALU == 1) {
      display.drawBitmap(0, 0, epd_bitmap_salu_small_2, 128, 128, 1);
    } else if (CURRENT_SALU == 2) {
      display.drawBitmap(0, 0, epd_bitmap_salu3, 128, 128, 1);
    } else if (CURRENT_SALU == 3) {
      display.drawBitmap(0, 0, edp_bitmap_salu4, 128, 128, 1);
    } else if (CURRENT_SALU == 4) {
      display.drawBitmap(0, 0, epd_bitmap_salu5, 128, 128, 1);
    } else if (CURRENT_SALU == 5) {
      display.drawBitmap(0, 0, epd_bitmap_salu6, 128, 128, 1);
    } else if (CURRENT_SALU == 6) {
      display.drawBitmap(0, 0, epd_bitmap_salu7, 128, 128, 1);
    } else if (CURRENT_SALU == 7) {
      display.drawBitmap(0, 0, epd_bitmap_salubook, 128, 128, 1);
    }
  }
  display.display();
}

void displayDiceSelect(int preset) {
  display.setTextSize(1);
  display.clearDisplay();
  drawPlusMinus(display);
  
  char title[32];
  (String(PRESET_NAMES[CURRENT_HIGHLIGHTED_PRESET]) + " - # OF DICE").toCharArray(title, SCREEN_HEIGHT / 2);
  drawTitle(display, title);

  display.setTextSize(2);

  char digitText[3];
  sprintf(digitText, "%d", CURRENT_DICE_AMOUNT);

  displayPrintCenter(display, digitText, SCREEN_HEIGHT / 2, true);
  display.display();
}

void displayRollScreen() {
  display.setTextSize(1);
  display.clearDisplay();
  //drawUISignifiers('<', '>', ' ');
  
  drawTitle(display, "SHAKE TO ROLL");

  displayDigits();

  char rollText[32];
  sprintf(rollText, "%d %s", CURRENT_DICE_AMOUNT, PRESET_NAMES[CURRENT_HIGHLIGHTED_PRESET]);

  if (RANDOM_NUMBERS[0] != -1 && RANDOM_NUMBERS[1] != -1 && !CURRENTLY_ROLLING) {
    char statLine[100] = "";

    display.setTextSize(2);
    createStatString(statLine);
    displayPrintCenter(display, statLine, SCREEN_HEIGHT - 20, true);
  } 
  if (CURRENTLY_ROLLING) {
    display.setTextSize(1);
    displayPrintCenter(display, "Rolling...", SCREEN_HEIGHT - 20, true);
  }

  display.setTextSize(1);
  displayPrintCenter(display, rollText, 16, true);
  displayPrintCenter(display, "SELECT TO RETURN", SCREEN_HEIGHT - 5, true);

  display.display();
}

void createStatString(char* strBuffer) {
	int min = 10000;
  int max = 0;
  int sum = 0;

  int index = 0;
  while (RANDOM_NUMBERS[index] != -1) {
    sum += RANDOM_NUMBERS[index];
    if (RANDOM_NUMBERS[index] < min) {
      min = RANDOM_NUMBERS[index];
    } else if (RANDOM_NUMBERS[index] > max) {
      max = RANDOM_NUMBERS[index];
    }
    index++;
  }

  sprintf(strBuffer, "Sum: %d", sum, min, max);
}

bool currentNumbersFitLarge() {
  int totalLengthOfDigits = 0;
  int index = 0;
  int maxCharLen = 0;

  while (RANDOM_NUMBERS[index] != -1) {
    totalLengthOfDigits += numberOfDigits(RANDOM_NUMBERS[index]) + 1;
    index++;
  }

  return totalLengthOfDigits <= LARGE_CHARS_PER_LINE*3;
}

void displayDigits() {
  // TODO: Refactor if time, this function is gross

  // Determines what size it needs to display
  int totalLengthOfDigits = 0;
  int numbersToDisplay = 0;
  int index = 0;

  int maxCharLen = 0;
  bool smallDisplay = currentNumbersFitLarge();

  // If total length of digits can fit on 8 chars over 2 lines, text size 2
  // Otherwise, text size 1
  if (smallDisplay) {
    maxCharLen = LARGE_CHARS_PER_LINE;
    display.setTextSize(2);
  } else {
    maxCharLen = SMALL_CHARS_PER_LINE;
    display.setTextSize(1);
  }

  char printLines[4][100] = {"", "", "", ""};
  char curDigit[maxCharLen+3] = "";

  int curLineLength = 0;
  int filledNumbers = 0;
  int curLine = 0;
  int numberIndex = 0;
  while (curLine < MAX_NUMBER_LINES && RANDOM_NUMBERS[numberIndex] != -1) {
    // Determine if the current number can fit in the current line
    int currentNumber = RANDOM_NUMBERS[numberIndex];
    int lengthOfCurrentNumber = numberOfDigits(currentNumber);

    if (curLineLength != 0) {
      lengthOfCurrentNumber++; // account for space
    }

    if (lengthOfCurrentNumber + curLineLength <= maxCharLen) {
      // add
      if (curLineLength != 0) {
        sprintf(curDigit, " %d", currentNumber);
      } else {
        sprintf(curDigit, "%d", currentNumber);
      }

      strcat(printLines[curLine], curDigit);
      curLineLength += lengthOfCurrentNumber;
      numberIndex++;
    } else {
      curLine++;
      curLineLength = 0;
      sprintf(curDigit, "");
    }
  }

  // Calculate height of line and number of lines
  short int x, y;
  unsigned short int textW, textH;
  int totalLines = 0;
  display.getTextBounds(printLines[0], 0, 0, &x, &y, &textW, &textH);

  for (int i = 0; i < 4; i++) {
    if (strlen(printLines[i]) > 0) {
      totalLines++;
    }
  }

  // Print each line
  int totalHeight = totalLines*textH;
  for (int i = 0; i < totalLines; i++) {
    int printY = (SCREEN_HEIGHT / 2) - (totalHeight / 2) + (textH * i);
    displayPrintCenter(display, printLines[i], printY, false);
  }
}

int numberOfDigits(int number) {
  int digits = 1;
  while (number / 10 != 0) {
    digits++;
    number = number / 10;
  }

  return digits;
}

void selectNextPreset() {
  int nextPreset = CURRENT_HIGHLIGHTED_PRESET + 1;
  
  if (nextPreset >= NumberOfPresets) {
    nextPreset = 0;
  }
  
  CURRENT_HIGHLIGHTED_PRESET = nextPreset;
}

void selectLastPreset() {
  int prevPreset = CURRENT_HIGHLIGHTED_PRESET - 1;

  if (prevPreset <= -1) {
    prevPreset = NumberOfPresets - 1;
  }

  CURRENT_HIGHLIGHTED_PRESET = prevPreset;
}


void loop() {
  if(digitalRead(LButPin) == HIGH && millis()-lastL>DEBOUNCE_TIME){
    handleLeftButtonPress();
    lastL = millis();  
  }
  
  if(digitalRead(RButPin) == HIGH && millis()-lastR>DEBOUNCE_TIME){
    handleRightButtonPress();
    lastR = millis();
  }
  
  if(digitalRead(SelButPin) == HIGH && millis()-lastS>DEBOUNCE_TIME){
    handleSelectPress();
    lastS = millis();
  }

  if(digitalRead(TempButPin) == HIGH && millis()-lastT>DEBOUNCE_TIME){
    handleShaken();
    lastT = millis();
  }
  
  delay(15);
}

void simpleRoll(int numberOfSides) {
  // 1. take the number of sides and number of dice state variables.
  // 2. Roll the numbers according to that and fill out the array
  int rollTicker = CURRENT_DICE_AMOUNT;
  while (rollTicker > 0) {
    RANDOM_NUMBERS[rollTicker-1] = random(1, numberOfSides+1);

    rollTicker -= 1 ;
  } 
  RANDOM_NUMBERS[CURRENT_DICE_AMOUNT] = -1;
}

void printNumberBuffer() {
  int index = 0;

  while (RANDOM_NUMBERS[index] != -1) {
    Serial.print(" ");
    Serial.print(RANDOM_NUMBERS[index]);

    index++;
  }

  Serial.println();
}

void simpleRollAnimation(int numberOfSides) {
  // 1. call the simple roll in a loop and show the numbers on the screen with a delay equaling to the parametric blend
  // 2. put that in a switch case which checks for if the roll needs to be simple or not.
  // 3. do complex roll....
  float numberOfRefreshes = 10.0;

  float delayTicker = 1.0;

  CURRENTLY_ROLLING = true;

  while (delayTicker < numberOfRefreshes){
    simpleRoll(numberOfSides);
    // print the current display buffer
    // printNumberBuffer();

    float currentDelay = (100.0 * delayBlend(delayTicker / numberOfRefreshes));

    displayCurrentContext();

    delay(currentDelay);
    delayTicker += 1;
  }

  delay(200.0);
  simpleRoll(numberOfSides);
  displayCurrentContext();

  CURRENTLY_ROLLING = false;

  delay(400.0);
  simpleRoll(numberOfSides);
  displayCurrentContext();
}

void displayCurrentRollAnimation() {
  switch(CURRENT_HIGHLIGHTED_PRESET){
    case D4:
      simpleRollAnimation(4);
      break;
    case D6:
      simpleRollAnimation(6);
      break;
    case D8:
      simpleRollAnimation(8);
      break;
    case D10:
      simpleRollAnimation(10);
      break;
    case D12:
      simpleRollAnimation(12);
      break;
    case D20:
      simpleRollAnimation(20);
      break;
    case D100:
      simpleRollAnimation(100);
      break;
  }

}

float delayBlend(float x)
{
  return -x * (x - 2);
  //return 0.5*x*(2-x)+0.5;
}

void handleShaken() {
  if (CURRENT_CONTEXT == ROLL_NUMBER && millis() - lastRoll > 1500){
    displayCurrentRollAnimation();
    lastRoll = millis();
  } else if (CURRENT_CONTEXT == SALU) {
    CURRENT_SALU = random(0, 7);
    displayCurrentContext();
  }
}
