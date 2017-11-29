#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <arm_math.h>
#include <Adafruit_GFX.h>
#include <Adafruit_DotStar.h>

#include "AudioVisualizer.h"
#include "constants.h"

#define MATRIX_SIZE         8
#define MATRIX_DATA_PIN     13
#define MATRIX_CLOCK_PIN    12

class Matrix : public Adafruit_GFX, public Adafruit_DotStar {

public:
    Matrix();

    void drawPicture(const uint8_t picture[]);
    void drawPictures(const uint8_t *pictures[], uint8_t frameIndex);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void fillScreen(uint16_t color);
    void initialize(AudioVisualizer pVisualizer);
    void loop();

    static uint16_t Color(uint8_t red, uint8_t green, uint8_t blue);

private:
    uint8_t colorIndex;
    uint8_t colorPosition;
    uint8_t state;
    int32_t frameIndex;
    long lastTime;
    uint8_t eyeDirection;
    long lastBlink;
    long lastStateChange;
    AudioVisualizer visualizer;

    void animate(const uint8_t *frames[], uint8_t numberOfFrames, uint32_t frameDuration);
    void renderEyes();
    void drawBars();
    void visualize();
    void writeText();
};

#endif
