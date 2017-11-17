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

#include <Adafruit_GFX.h>
#include <Adafruit_DotStar.h>

#include "AudioVisualizer.h"
#include "Matrix.h"
#include "Strip.h"
#include "graphics.h"

AudioVisualizer visualizer = AudioVisualizer();
Matrix matrix = Matrix();
Strip strip = Strip();

void setup() {
    Serial.begin(115200);
    visualizer.initialize();
    matrix.initialize(visualizer);
    strip.initialize(visualizer);
}

void loop() {
    visualizer.loop();
    matrix.loop();
    strip.loop();
}
