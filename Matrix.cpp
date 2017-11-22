#include <Adafruit_DotStar.h>

#include "AudioVisualizer.h"
#include "Matrix.h"
#include "gamma.h"
#include "graphics.h"

#define TOTAL_STATES        3
#define STATE_VISUALIZE     0
#define STATE_BEER          1
#define STATE_EYES          2

#define NUMBER_OF_FRAMES 3
#define FRAME_DURATION 300

#define BEER_FRAMES 1
const uint8_t *beerAnimation[] = {
    BEER, BEER
};

#define EYE_POSITIONS 5

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
    return ((uint16_t)(green & 0xF8) << 8) |
           ((uint16_t)(red & 0xFC) << 3) |
                      (blue >> 3);
}

void Matrix::initialize(AudioVisualizer pVisualizer) {
    visualizer = pVisualizer;
    state = STATE_EYES;

    begin();
    setTextWrap(false);
    setBrightness(24);
    fillScreen(0);
    show();

    frameIndex = 0;
    lastTime = 0;
    lastBlink = millis();
    lastStateChange = millis();
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
        red = pgm_read_byte(&picture[i * 3]);
        green = pgm_read_byte(&picture[i * 3 + 1]);
        blue = pgm_read_byte(&picture[i * 3 + 2]);
        drawPixel(i % MATRIX_SIZE, i / MATRIX_SIZE, Matrix::Color(red, green, blue));
        drawPixel(i % MATRIX_SIZE + MATRIX_SIZE, i / MATRIX_SIZE, Matrix::Color(red, green, blue));
    }
}

void Matrix::drawPictures(const uint8_t *pictures[], uint8_t frameIndex) {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t i;

    for (uint8_t i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        red = pgm_read_byte(&pictures[frameIndex * 2][i * 3]);
        green = pgm_read_byte(&pictures[frameIndex * 2][i * 3 + 1]);
        blue = pgm_read_byte(&pictures[frameIndex * 2][i * 3 + 2]);
        drawPixel(i % MATRIX_SIZE, i / MATRIX_SIZE, Matrix::Color(red, green, blue));

        red = pgm_read_byte(&pictures[frameIndex * 2 + 1][i * 3]);
        green = pgm_read_byte(&pictures[frameIndex * 2 + 1][i * 3 + 1]);
        blue = pgm_read_byte(&pictures[frameIndex * 2 + 1][i * 3 + 2]);
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
    switch (state) {
        case STATE_VISUALIZE:
            visualize();
            break;
        case STATE_BEER:
            animate(beerAnimation, BEER_FRAMES, FRAME_DURATION);
            break;
        case STATE_EYES:
            renderEyes();
            break;
        default:
            visualize();
            break;
    }

    if (millis() - lastStateChange > 5000) {
        uint8_t shouldChange = random(max(1, 30000 - (millis() - lastStateChange)));
        if (shouldChange == 0) {
            frameIndex = 0;
            state = (++state) % TOTAL_STATES;
            lastStateChange = millis();
        }
    }
}

void Matrix::visualize() {
    clear();

    float32_t *output = visualizer.getSmoothedOutput();
    float32_t maximum = visualizer.getAverageMaximumValue();
    if (maximum == 0) {
        maximum = 100;
    }
    for (int i = 0; i < 16; i++) {
        float32_t value = 8 - (output[i] / maximum) * 8;
        if (value > 7) {
            drawLine(i, 0, i, 7, 0);
            continue;
        }

        drawLine(i, max(0, round(value) - 1), i, 7, Matrix::Color(255, 0, 0));
    }

    show();
}

void Matrix::renderEyes() {
    if (millis() - lastTime < 32) {
        return;
    }

    clear();

    uint8_t shouldChange = random(eyeDirection == 0 ? 4 : 16);
    if (shouldChange == 0) {
        eyeDirection = random(12) + 1;
    }

    if (millis() - lastBlink > 3000) {
        uint8_t shouldBlink = random(max(1, 6000 - (millis() - lastBlink)));
        if (shouldBlink == 0) {
            eyeDirection = 0;
            lastBlink = millis();
        }
    }

    switch (eyeDirection) {
        case 0:
            drawPicture(EYE_BLINK);
            break;
        case 1:
            drawPicture(EYE_LEFT);
            break;
        case 2:
            drawPicture(EYE_RIGHT);
            break;
        case 3:
            drawPicture(EYE_UP);
            break;
        case 4:
            drawPicture(EYE_DOWN);
            break;
        case 5:
            lastBlink = millis();
            drawPicture(EYE_BLINK);
            break;
        default:
            drawPicture(EYE_CENTER);
            break;
    }

    show();

    lastTime = millis();
}

void Matrix::animate(const uint8_t *frames[], uint8_t numberOfFrames, uint32_t frameDuration) {
    if (millis() - lastTime < frameDuration) {
        return;
    }

    clear();
    frameIndex++;
    if (frameIndex >= numberOfFrames) {
        frameIndex = 0;
    }

    drawPictures(frames, frameIndex);
    show();

    lastTime = millis();
}
