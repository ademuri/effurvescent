#include "Arduino.h"
#include <FastLED.h>

const unsigned int TOUCH_R_PIN = 15;
const unsigned int TOUCH_G_PIN = 16;
const unsigned int TOUCH_B_PIN = 18;

const float TOUCH_THRESH_MULTIPLIER = 2.0;

int touchRBaseline = 0;
int touchGBaseline = 0;
int touchBBaseline = 0;

const unsigned int NUM_LEDS = 60;
const unsigned int LEDS_PER_COLUMN = 12;
const unsigned int LED_OUT_PIN = 17;

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(57600);

  touchRead(TOUCH_R_PIN);
  touchRead(TOUCH_G_PIN);
  touchRead(TOUCH_B_PIN);
  delay(10);
  touchRBaseline = touchRead(TOUCH_R_PIN) * TOUCH_THRESH_MULTIPLIER;
  touchGBaseline = touchRead(TOUCH_G_PIN) * TOUCH_THRESH_MULTIPLIER;
  touchBBaseline = touchRead(TOUCH_B_PIN) * TOUCH_THRESH_MULTIPLIER;

  FastLED.addLeds<NEOPIXEL, LED_OUT_PIN>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 400);
  FastLED.setBrightness(64);
  //FastLED.setDither( 0 );

  setAllColor(CRGB::Black);
  /*setAllColor(CRGB::Red);
  delay(500);
  setAllColor(CRGB::Green);
  delay(500);
  setAllColor(CRGB::Blue);
  delay(500);
  setAllColor(CRGB::Black);
  delay(500);*/
}

void setAllColor(CRGB color) {
  for (unsigned int i=0; i<NUM_LEDS; i++) {
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

void loop() {
	for (unsigned long i=0; i<12; i++) {
		setPixelColor(0, i, CHSV(sin8(colorCounter + i * 2) - 6, 255, addWithCeiling(cubicwave8(moveCounter + i * MOVE_SIZE), MOVE_DC_OFFSET)));
		setPixelColor(1, i, CHSV(sin8(colorCounter + i * 2) - 3, 255, addWithCeiling(cubicwave8(moveCounter + i * MOVE_SIZE), MOVE_DC_OFFSET)));
		setPixelColor(2, i, CHSV(sin8(colorCounter + i * 2), 255, addWithCeiling(cubicwave8(moveCounter + i * MOVE_SIZE), MOVE_DC_OFFSET)));
		setPixelColor(3, i, CHSV(sin8(colorCounter + i * 2) - 3, 255, addWithCeiling(cubicwave8(moveCounter + i * MOVE_SIZE), MOVE_DC_OFFSET)));
		setPixelColor(4, i, CHSV(sin8(colorCounter + i * 2) - 6, 255, addWithCeiling(cubicwave8(moveCounter + i * MOVE_SIZE), MOVE_DC_OFFSET)));
	}

	FastLED.show();

	colorSubCounter++;
	if (colorSubCounter > COLOR_STEPS) {
		colorSubCounter = 0;
		colorCounter++;
	}
	moveCounter += 2;
	FastLED.delay(50);

  /*if (touchRead(TOUCH_R_PIN) > touchRBaseline) {
    setAllColor(CRGB::Red);
  } else if (touchRead(TOUCH_G_PIN) > touchGBaseline) {
    setAllColor(CRGB::Green);
  } else if (touchRead(TOUCH_B_PIN) > touchBBaseline) {
    setAllColor(CRGB::Blue);
  }*/
}
