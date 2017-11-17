#include <Adafruit_DotStar.h>

#include "Strip.h"

#define FRAME_DURATION 20

Strip::Strip()
    : Adafruit_DotStar(LED_STRIP_PIXELS, LED_STRIP_DATA_PIN, LED_STRIP_CLOCK_PIN, DOTSTAR_BRG)
{
    head = 0;
    tail = -8;
}

// Downgrade 24-bit color to 16-bit (add reverse gamma lookup here?)
uint32_t Strip::Color(uint8_t red, uint8_t green, uint8_t blue)
{
    return ((uint32_t)green << 16) | ((uint32_t)red << 8) | blue;
}

uint16_t Strip::colorWheel(uint8_t position) {
    position = 255 - position;
    if(position < 85)
    {
        return Strip::Color(255 - position * 3, 0, position * 3);
    }
    else if(position < 170)
    {
        position -= 85;
        return Strip::Color(0, position * 3, 255 - position * 3);
    }
    else
    {
        position -= 170;
        return Strip::Color(position * 3, 255 - position * 3, 0);
    }
}

void Strip::initialize(AudioVisualizer pVisualizer) {
    visualizer = pVisualizer;

    begin();
    setBrightness(128);
    clear();
    show();

    lastTime = millis();
}

void Strip::loop() {
    if (millis() - lastTime < FRAME_DURATION) {
        return;
    }

    setPixelColor(head, colorWheel(position));
    setPixelColor(tail, 0);
    show();

    if (++head >= LED_STRIP_PIXELS) {
        head = 0;
    }

    if (++tail >= LED_STRIP_PIXELS) {
        tail = 0;
    }

    position++;
    lastTime = millis();
}
