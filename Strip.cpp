#include <Adafruit_DotStar.h>

#include "Strip.h"

#define FRAME_DURATION 8

Strip::Strip()
    : Adafruit_DotStar(LED_STRIP_PIXELS, LED_STRIP_DATA_PIN, LED_STRIP_CLOCK_PIN, DOTSTAR_BRG)
{
    brightness = 16;
    largestRead = 1;
}

uint32_t Strip::Color(uint8_t red, uint8_t green, uint8_t blue)
{
    return ((uint32_t)green << 16) | ((uint32_t)red << 8) | blue;
}

uint32_t Wheel(byte WheelPos) {
    if(WheelPos < 85) {
        return Strip::Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    } else if(WheelPos < 170) {
        WheelPos -= 85;
        return Strip::Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } else {
        WheelPos -= 170;
        return Strip::Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
}

void Strip::initialize(AudioVisualizer pVisualizer) {
    visualizer = pVisualizer;
    lastBeat = millis();

    begin();
    setBrightness(8);
    clear();
    show();

    lastTime = millis();
}

void Strip::loop() {
    calculateBeat();
    cycle();

    show();
}

void Strip::calculateBeat() {
    float32_t *output = visualizer.getEqualizedOutput();

    float32_t reads[previousReads.size()];
    for (int i = 0; i < previousReads.size(); i++) {
        reads[i] = previousReads.at(i);
    }
    float32_t avg = 1;
    arm_mean_f32(reads, previousReads.size(), &avg);

    float32_t sample = output[0] + output[1];
    float32_t threshold = max(largestRead * 0.8, (avg * 1.5));

    if (sample > threshold) {
        uint8_t nextBrightness = min(228, max(64, round(255 * ((sample - avg) / sample))));
        position += round((millis() - lastBeat) / 1000) + random(5, 15);
        lastBeat = millis();
        if (nextBrightness > brightness) {
            brightness = nextBrightness;
        }
    } else {
        largestRead = max(0.5, largestRead - 0.05);
        brightness = max(16, brightness - 20);
    }

    if (previousReads.size() == 64) {
        previousReads.pop_front();
    }

    if (sample > largestRead) {
        largestRead = sample;
    }

    previousReads.push_back(sample);

    setBrightness(brightness);
}

void Strip::cycle() {
    if (millis() - lastTime < FRAME_DURATION) {
        return;
    }

    uint8_t index;
    for (index = 0; index < LED_STRIP_PIXELS; index++) {
        setPixelColor(index, Wheel(position + index));
    }

    position++;
    lastTime = millis();
}
