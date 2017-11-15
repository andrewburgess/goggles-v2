#include <Adafruit_DotStar.h>

#include "AudioVisualizer.h"
#include "Matrix.h"
#include "gamma.h"
#include "graphics.h"

const uint8_t *FRAMES[] = {BEER, CHERRIES, MARIO};
#define NUMBER_OF_FRAMES 3
#define FRAME_DURATION 300

// Two matrix boards of 8x8, tiled horizontally
Matrix::Matrix()
    : Adafruit_GFX(MATRIX_SIZE * 2, MATRIX_SIZE),
      Adafruit_DotStar(MATRIX_SIZE * 2 * MATRIX_SIZE, MATRIX_DATA_PIN, MATRIX_CLOCK_PIN, DOTSTAR_BRG)
{
}

// Expand 16-bit input color (Adafruit_GFX colorspace) to 24-bit (DotStar)
// (w/gamma adjustment)
static uint32_t expandColor(uint16_t color)
{
    return ((uint32_t)pgm_read_byte(&gamma5[color >> 11 ]) << 16) |
           ((uint32_t)pgm_read_byte(&gamma6[(color >> 5) & 0x3F]) << 8) |
           pgm_read_byte(&gamma5[color & 0x1F]);
}

// Downgrade 24-bit color to 16-bit (add reverse gamma lookup here?)
uint16_t Matrix::Color(uint8_t red, uint8_t green, uint8_t blue)
{
    return ((uint16_t)(red & 0xF8) << 8) |
           ((uint16_t)(green & 0xFC) << 3) |
                      (blue >> 3);
}

void Matrix::initialize(AudioVisualizer pVisualizer) {
    visualizer = pVisualizer;

    begin();
    setTextWrap(false);
    setBrightness(24);
    fillScreen(0);
    show();

    frameIndex = 0;
    lastTime = millis();
}

void Matrix::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    int pixelPosition;

    if (x >= MATRIX_SIZE) // Pixel is on second matrix board
    {
        // 0,0 is the upper right of this board, so need to remap that to
        // 8,0 (pixel #71)
        pixelPosition = 71 + (y * MATRIX_SIZE) - (x - MATRIX_SIZE);
    }
    else // Pixel is on first matrix board
    {
        // 0,0 is technically the bottom right of the board, so need to remap
        // that to 8,8 (pixel #63)
        pixelPosition = (63 - (MATRIX_SIZE * x + y));
    }

    setPixelColor(pixelPosition, expandColor(color));
}

void Matrix::drawPicture(const uint8_t picture[]) {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    for (uint8_t i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        green = pgm_read_byte(&picture[i * 3]);
        red = pgm_read_byte(&picture[i * 3 + 1]);
        blue = pgm_read_byte(&picture[i * 3 + 2]);
        drawPixel(i % MATRIX_SIZE, i / MATRIX_SIZE, Matrix::Color(red, green, blue));
        drawPixel(i % MATRIX_SIZE + MATRIX_SIZE, i / MATRIX_SIZE, Matrix::Color(red, green, blue));
    }
}

void Matrix::fillScreen(uint16_t color)
{
    uint16_t i, pixels;
    uint32_t expandedColor = expandColor(color);

    pixels = numPixels();
    for (i = 0; i < pixels; i++)
    {
        setPixelColor(i, expandedColor);
    }
}

void Matrix::loop() {
    clear();

    float32_t *output = visualizer.getOutput();
    int32_t count = visualizer.getSampleCount();
    for (int i = 0; i < count; i++) {
        int32_t red = min(255, 5 + ((output[i] / 2000) * 255));
        drawPixel(i / 16, i % 8, Matrix::Color(red, 0, 0));
    }

    show();
}

void animation() {
    /*if (millis() - lastTime < FRAME_DURATION) {
        return;
    }

    clear();
    frameIndex++;
    if (frameIndex >= NUMBER_OF_FRAMES) {
        frameIndex = 0;
    }

    drawPicture(FRAMES[frameIndex]);
    show();

    lastTime = millis();*/
}
