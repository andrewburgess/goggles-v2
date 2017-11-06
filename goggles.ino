/******************************************************************************

GOGGLES V2

A new version of lightup goggles, using an LED strip and two 8x8 LED matrices

Hardware:
    - FLORA microcontroller (https://www.adafruit.com/product/659)
    - Electret Microphone Amplifier (https://www.adafruit.com/product/1713)
    - DotStar LED Strip - 60 LED / meter (https://www.adafruit.com/product/2239)
    - 2x DotStar 8x8 Grid (https://www.adafruit.com/product/3444)

Software Libraries:
    - Adafruit_DotStar (https://github.com/adafruit/Adafruit_DotStar)
    - Adafruit_DotStarMatrix (https://github.com/adafruit/Adafruit_DotStarMatrix)
    - Adafruit-GFX-Library (https://github.com/adafruit/Adafruit-GFX-Library)

******************************************************************************/

#include <avr/pgmspace.h>
#include <SPI.h>

#include <Adafruit_GFX.h>
#include <Adafruit_DotStar.h>
#include <Adafruit_DotStarMatrix.h>

#include "bitmap/heart.h"

#define MATRIX_SIZE         8
#define MATRIX_DATA_PIN     4
#define MATRIX_CLOCK_PIN    5

#define LED_STRIP_PIXELS    16
#define LED_STRIP_DATA_PIN  2
#define LED_STRIP_CLOCK_PIN 3

Adafruit_DotStarMatrix matrix = Adafruit_DotStarMatrix(
                                    MATRIX_SIZE,
                                    MATRIX_SIZE,
                                    2,
                                    1,
                                    MATRIX_DATA_PIN,
                                    MATRIX_CLOCK_PIN,
                                    DS_MATRIX_TOP + DS_MATRIX_RIGHT +
                                    DS_MATRIX_COLUMNS + DS_MATRIX_PROGRESSIVE,
                                    DOTSTAR_BRG
                                );
Adafruit_DotStar strip = Adafruit_DotStar(LED_STRIP_PIXELS, LED_STRIP_DATA_PIN, LED_STRIP_CLOCK_PIN)

void setupMatrix() {
    matrix.begin();
    matrix.setTextWrap(false);
    matrix.setBrightness(16);
    matrix.fillScreen(0);
    matrix.show();
}

void setupStrip() {
    strip.begin();
    strip.setBrightness(16);
    strip.clear();
    strip.show();
}

void setup() {
    setupMatrix();
    setupStrip();
}

void loop() {
    matrix.drawXBitmap(0, 0, heart, 8, 8, 0xFF0000);
    matrix.show();
}
