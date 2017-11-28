#include <Adafruit_DotStar.h>

#include "AudioVisualizer.h"
#include "Matrix.h"
#include "gamma.h"
#include "graphics.h"

#define TOTAL_STATES        4
#define STATE_VISUALIZE     0
#define STATE_BEER          1
#define STATE_EYES          2
#define STATE_TEXT          3

#define NUMBER_OF_FRAMES        3
#define COLUMN_AVERAGE_FRAMES   10
#define FRAME_DURATION          300

#define BEER_FRAMES 1
const uint8_t *beerAnimation[] = {
    BEER, BEER
};

#define EYE_POSITIONS 5

static const float32_t column0[]  = { 2, 0, 0.75, 0.25 };                                                               // 0        0, 1
static const float32_t column1[]  = { 2, 1, 0.65, 0.35 };                                                               // 1        1, 2
static const float32_t column2[]  = { 3, 1, 0.16, 0.50, 0.34 };                                                         // 2        1, 2, 3
static const float32_t column3[]  = { 4, 2, 0.08, 0.42, 0.38, 0.12 };                                                   // 3,4      2, 3, 4, 5
static const float32_t column4[]  = { 4, 2, 0.02, 0.05, 0.43, 0.53 };                                                   // 4,5      2, 3, 4, 5
static const float32_t column5[]  = { 4, 3, 0.12, 0.28, 0.50, 0.20 };                                                   // 5        3, 4, 5, 6
static const float32_t column6[]  = { 5, 4, 0.08, 0.12, 0.32, 0.30, 0.18 };                                             // 6,7      4, 5, 6, 7, 8
static const float32_t column7[]  = { 6, 6, 0.06, 0.08, 0.28, 0.32, 0.16, 0.10 };                                       // 8,9      6, 7, 8, 9, 10, 11
static const float32_t column8[]  = { 6, 8, 0.03, 0.07, 0.42, 0.34, 0.09, 0.05 };                                       // 10,11    8, 9, 10, 11, 12, 13
static const float32_t column9[]  = { 6, 10, 0.05, 0.09, 0.32, 0.34, 0.11, 0.09 };                                      // 12,13    10, 11, 12, 13, 14, 15
static const float32_t column10[] = { 6, 12, 0.05, 0.09, 0.32, 0.34, 0.11, 0.09 };                                      // 14,15    12, 13, 14, 15, 16, 17
static const float32_t column11[] = { 6, 14, 0.05, 0.09, 0.32, 0.34, 0.11, 0.09 };                                      // 16,17    14, 15, 16, 17, 18, 19
static const float32_t column12[] = { 6, 16, 0.05, 0.09, 0.32, 0.34, 0.11, 0.09 };                                      // 18,19    16, 17, 18, 19, 20, 21
static const float32_t column13[] = { 6, 18, 0.05, 0.09, 0.32, 0.34, 0.11, 0.09 };                                      // 20,21    18, 19, 20, 21, 22, 23
static const float32_t column14[] = { 6, 20, 0.05, 0.09, 0.32, 0.34, 0.11, 0.09 };                                      // 22,23    20, 21, 22, 23, 24, 25
static const float32_t column15[] = { 6, 22, 0.05, 0.09, 0.32, 0.34, 0.11, 0.09 };                                      // 24,25    22, 23, 24, 25, 26, 27

static const float32_t *columnData[] = {
                                        column0, column1, column2, column3,
                                        column4, column5, column6, column7,
                                        column8, column9, column10, column11,
                                        column12, column13, column14, column15
                                    };

uint32_t columnDivider[16];
uint8_t dotCounter;
uint8_t peak[16];
float32_t columns[16][COLUMN_AVERAGE_FRAMES]; // Column levels for previous 10 frames
float32_t minimumAverageLevel[16]; // Used for dynamically adjusting
float32_t maximumAverageLevel[16]; // pseudo rolling averages for prior frames

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
    state = STATE_VISUALIZE;

    begin();
    setTextWrap(false);
    setBrightness(24);
    fillScreen(0);
    show();

    frameIndex = 0;
    lastTime = 0;
    lastBlink = millis();
    lastStateChange = millis();

    uint8_t i;
    for (i = 0; i < 16; i++) {
        minimumAverageLevel[i] = 0;
        maximumAverageLevel[i] = 1;
    }
}

void Matrix::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if ((x < 0 || y < 0) || (x >= MATRIX_SIZE * 2 || y >= MATRIX_SIZE)) return;

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
        case STATE_TEXT:
            writeText();
            break;
        default:
            visualize();
            break;
    }

    /*if (millis() - lastStateChange > 5000) {
        uint8_t shouldChange = random(max(1, 10000 - (millis() - lastStateChange)));
        if (shouldChange == 0) {
            frameIndex = 0;
            state = random(0, TOTAL_STATES - 1);
            lastStateChange = millis();
        }
    }*/
}

void Matrix::visualize() {
    clear();

    fillRect(0, 0, 16, 3, Matrix::Color(255, 0, 0));
    fillRect(0, 3, 16, 2, Matrix::Color(255, 255, 0));
    fillRect(0, 5, 16, 3, Matrix::Color(0, 255, 0));

    float32_t *data;
    float32_t *output = visualizer.getSmoothedOutput();
    uint8_t i, c, x, y;
    float32_t volume, minimumLevel, maximumLevel, level;
    uint8_t numberOfBins;
    uint8_t startBin;

    float32_t average = visualizer.getAverageValue();
    float32_t maximum = visualizer.getLastMaximumValue();

    for (x = 0; x < 16; x++) {
        level = 0;
        volume = 0;
        minimumLevel = 0;
        maximumLevel = 0;

        data = (float32_t *)columnData[x];
        numberOfBins = data[0];
        startBin = data[1];

        for (i = 0; i < numberOfBins; i++) {
            volume += output[startBin + i] * data[i + 2];
        }

        columns[x][frameIndex] = volume;
        minimumLevel = maximumLevel = columns[x][0];
        for (i = 0; i < COLUMN_AVERAGE_FRAMES; i++) {
            if (columns[x][i] < minimumLevel)       minimumLevel = columns[x][i];
            else if (columns[x][i] > maximumLevel)  maximumLevel = columns[x][i];
        }

        if ((maximumLevel - minimumLevel) < max(0.15, (maximum - average))) {
            maximumLevel = minimumLevel + max(0.15, (maximum - average));
        }

        minimumAverageLevel[x] = (minimumAverageLevel[x] + minimumLevel) / 2;
        maximumAverageLevel[x] = (maximumAverageLevel[x] + maximumLevel) / 2;

        level = 10.0f * (columns[x][frameIndex] - minimumAverageLevel[x]) / (maximumAverageLevel[x] - minimumAverageLevel[x]);

        if (level < 0)       c = 0;
        else if (level > 10) c = 10;
        else                 c = (uint8_t)(round(level));

        if (c > peak[x]) peak[x] = c;

        if (peak[x] <= 0) {
            drawLine(x, 0, x, 7, Matrix::Color(0, 0, 0));
            continue;
        } else if (c < 8) {
            drawLine(x, 0, x, 7 - c, Matrix::Color(0, 0, 0));
        }

        y = 7 - peak[x];
        if (y < 2)      drawPixel(x, y, Matrix::Color(255, 0, 0));
        else if (y < 6) drawPixel(x, y, Matrix::Color(255, 255, 0));
        else            drawPixel(x, y, Matrix::Color(0, 255, 0));
    }

    show();

    if (++dotCounter >= 2) {
        dotCounter = 0;
        for (x = 0; x < 16; x++) {
            if (peak[x] > 0) peak[x]--;
        }
    }

    if (++frameIndex >= COLUMN_AVERAGE_FRAMES) frameIndex = 0;
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

    if (millis() - lastBlink > 2000) {
        uint8_t shouldBlink = random(max(1, 4000 - (millis() - lastBlink)));
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

void Matrix::writeText() {
    if (millis() - lastTime < 32) {
        return;
    }

    lastTime = millis();

    setTextColor(Matrix::Color(255, 0, 0));
    setTextWrap(false);

    clear();
    setCursor(frameIndex, 0);
    print(F("REZZ 4 EVER"));
    show();

    if (--frameIndex < -86) {
        frameIndex = width();
    }
}
