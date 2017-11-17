#ifndef _MATRIX_H_
#define _MATRIX_H_

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
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void fillScreen(uint16_t color);
    void initialize(AudioVisualizer pVisualizer);
    void loop();

    static uint16_t Color(uint8_t red, uint8_t green, uint8_t blue);

private:
    uint8_t frameIndex;
    long lastTime;
    AudioVisualizer visualizer;
};

#endif
