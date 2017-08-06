#include "Arduino.h"
#include <FastLED.h>

const unsigned int FAST_PIN = 15;
const unsigned int SLOW_PIN = 16;
const unsigned int BRIGHT_PIN = 18;

enum ButtonState { BUTTON_NONE, BUTTON_FAST, BUTTON_SLOW, BUTTON_BRIGHT };

enum Pattern {
  PATTERN_SLOW,
  PATTERN_FAST,
};

unsigned int brightnessIndex = 3;
unsigned int BRIGHTNESS_TABLE[] = {32, 50, 64, 80};
unsigned int BRIGHTNESS_COUNT = 4;

const float TOUCH_THRESH_MULTIPLIER = 2.0;

int fastBaseline = 0;
int slowBaseline = 0;
int brightBaseline = 0;

const unsigned int NUM_LEDS = 60;
const unsigned int LEDS_PER_COLUMN = 12;
const unsigned int LED_OUT_PIN = 17;

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(57600);

  touchRead(FAST_PIN);
  touchRead(SLOW_PIN);
  touchRead(BRIGHT_PIN);
  delay(10);
  fastBaseline = touchRead(FAST_PIN) * TOUCH_THRESH_MULTIPLIER;
  slowBaseline = touchRead(SLOW_PIN) * TOUCH_THRESH_MULTIPLIER;
  brightBaseline = touchRead(BRIGHT_PIN) * TOUCH_THRESH_MULTIPLIER;

  FastLED.addLeds<NEOPIXEL, LED_OUT_PIN>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 400);
  FastLED.setBrightness(64);
  // FastLED.setDither( 0 );

  setAllColor(CRGB::Black);
}

void setAllColor(CRGB color) {
  for (unsigned int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
}

void setRowColor(int row, CRGB color) {
  leds[LEDS_PER_COLUMN - row - 1] = color;
  leds[LEDS_PER_COLUMN + row] = color;
  leds[LEDS_PER_COLUMN * 3 - row - 1] = color;
  leds[LEDS_PER_COLUMN * 3 + row] = color;
  leds[LEDS_PER_COLUMN * 5 - row - 1] = color;
}

void setPixelColor(int col, int row, CRGB color) {
  switch (col) {
  case 0:
    leds[LEDS_PER_COLUMN - row - 1] = color;
    break;
  case 1:
    leds[LEDS_PER_COLUMN + row] = color;
    break;
  case 2:
    leds[LEDS_PER_COLUMN * 3 - row - 1] = color;
    break;
  case 3:
    leds[LEDS_PER_COLUMN * 3 + row] = color;
    break;
  case 4:
    leds[LEDS_PER_COLUMN * 5 - row - 1] = color;
    break;
  }
}

unsigned int colorCounter = 0;
unsigned int colorSubCounter = 0;
const unsigned int COLOR_STEPS = 2;
unsigned int moveCounter = 0;
unsigned int moveSubCounter = 0;
const unsigned int MOVE_STEPS = 2;
const unsigned int MOVE_SIZE = 40;
const unsigned int MOVE_DC_OFFSET = 40;

unsigned int addWithCeiling(unsigned int a, unsigned int b) {
  if (0l + a + b > 255) {
    return 255;
  }
  return a + b;
}

ButtonState buttonState = BUTTON_NONE;
ButtonState prevButtonState = BUTTON_NONE;
Pattern currentPattern = PATTERN_SLOW;

const unsigned long BUTTON_PRESS_DELAY = 500;
unsigned long buttonPressAt = 0;

void doSlowPattern() {
  for (unsigned long i = 0; i < 12; i++) {
    setPixelColor(0, i,
                  CHSV(sin8(colorCounter + i * 2) - 6, 255,
                       addWithCeiling(cubicwave8(moveCounter + i * MOVE_SIZE),
                                      MOVE_DC_OFFSET)));
    setPixelColor(1, i,
                  CHSV(sin8(colorCounter + i * 2) - 3, 255,
                       addWithCeiling(cubicwave8(moveCounter + i * MOVE_SIZE),
                                      MOVE_DC_OFFSET)));
    setPixelColor(2, i,
                  CHSV(sin8(colorCounter + i * 2), 255,
                       addWithCeiling(cubicwave8(moveCounter + i * MOVE_SIZE),
                                      MOVE_DC_OFFSET)));
    setPixelColor(3, i,
                  CHSV(sin8(colorCounter + i * 2) - 3, 255,
                       addWithCeiling(cubicwave8(moveCounter + i * MOVE_SIZE),
                                      MOVE_DC_OFFSET)));
    setPixelColor(4, i,
                  CHSV(sin8(colorCounter + i * 2) - 6, 255,
                       addWithCeiling(cubicwave8(moveCounter + i * MOVE_SIZE),
                                      MOVE_DC_OFFSET)));
  }

  FastLED.show();

  colorSubCounter++;
  if (colorSubCounter > COLOR_STEPS) {
    colorSubCounter = 0;
    colorCounter--;
  }
  moveCounter += 2;
}

void doFastPattern() {
  static boolean checker = false;
  CHSV onColor;
  CHSV offColor;
  if (checker) {
    onColor = CHSV(0, 255, 255);
    offColor = CHSV(0, 255, 0);
  } else {
    onColor = CHSV(0, 255, 0);
    offColor = CHSV(0, 255, 255);
  }

  for (int i = 0; i < 3; i++) {
    setRowColor(i, onColor);
  }
  for (int i = 3; i < 6; i++) {
    setRowColor(i, offColor);
  }
  for (int i = 6; i < 9; i++) {
    setRowColor(i, onColor);
  }
  for (int i = 9; i < 12; i++) {
    setRowColor(i, offColor);
  }

  FastLED.show();
  checker = !checker;
}

void loop() {
  switch (currentPattern) {
  case PATTERN_SLOW:
  default:
    doSlowPattern();
    break;

  case PATTERN_FAST:
    doFastPattern();
    break;
  }
  FastLED.delay(50);

  bool slow = touchRead(SLOW_PIN) > slowBaseline;
  bool fast = touchRead(FAST_PIN) > fastBaseline;
  bool bright = touchRead(BRIGHT_PIN) > brightBaseline;

  if (slow && fast || slow && bright || fast && bright) {
    // More than one button pressed - ignore
    buttonState = BUTTON_NONE;
  } else {
    if (slow) {
      buttonState = BUTTON_SLOW;
    } else if (fast) {
      buttonState = BUTTON_FAST;
    } else if (bright) {
      buttonState = BUTTON_BRIGHT;
    } else {
      buttonState = BUTTON_NONE;
    }
  }

  if (buttonState != prevButtonState) {
    buttonPressAt = millis() + BUTTON_PRESS_DELAY;
  } else {
    if (millis() > buttonPressAt) {
      switch (buttonState) {
      case BUTTON_SLOW:
        currentPattern = PATTERN_SLOW;
        buttonPressAt = millis() + BUTTON_PRESS_DELAY;
        break;
      case BUTTON_FAST:
        currentPattern = PATTERN_FAST;
        buttonPressAt = millis() + BUTTON_PRESS_DELAY;
        break;
      case BUTTON_BRIGHT:
        brightnessIndex = (brightnessIndex + 1) % BRIGHTNESS_COUNT;
        FastLED.setBrightness(BRIGHTNESS_TABLE[brightnessIndex]);

        buttonPressAt = millis() + BUTTON_PRESS_DELAY;
        break;
      case BUTTON_NONE:
      default:
        // No-op
        break;
      }
    }
  }

  prevButtonState = buttonState;
}
