#include <Adafruit_DotStar.h>

#include "AudioVisualizer.h"
#include "Matrix.h"
#include "gamma.h"
#include "graphics.h"

#define TOTAL_STATES            5
#define STATE_VISUALIZE         0
#define VISUALIZE_DURATION      60000
#define STATE_COLOR_SWIRL       1
#define COLOR_SWIRL_DURATION    15000
#define STATE_EYES              2
#define EYES_DURATION           10000
#define STATE_TEXT              3
#define TEXT_DURATION           8000
#define STATE_HEART             4
#define HEART_DURATION          10000
#define STATE_BEER              5
#define BEER_DURATION           1000

#define NUMBER_OF_FRAMES        3
#define COLUMN_AVERAGE_FRAMES   6
#define FRAME_DURATION          8

#define BEER_FRAMES 1
const uint8_t *beerAnimation[] = {
    BEER, BEER
};

#define COLOR_SWIRL_FRAMES 8
const uint8_t *colorSwirlAnimation[] = {
    COLOR_SWIRL_1_LEFT, COLOR_SWIRL_1_RIGHT,
    COLOR_SWIRL_1_LEFT, COLOR_SWIRL_1_RIGHT,
    COLOR_SWIRL_2_LEFT, COLOR_SWIRL_2_RIGHT,
    COLOR_SWIRL_2_LEFT, COLOR_SWIRL_2_RIGHT,
    COLOR_SWIRL_3_LEFT, COLOR_SWIRL_3_RIGHT,
    COLOR_SWIRL_3_LEFT, COLOR_SWIRL_3_RIGHT,
    COLOR_SWIRL_4_LEFT, COLOR_SWIRL_4_RIGHT,
    COLOR_SWIRL_4_LEFT, COLOR_SWIRL_4_RIGHT
};

#define EYE_POSITIONS 5

/*static const float32_t column0[]  = { 1, 0, 1.0 };
static const float32_t column1[]  = { 2, 0, 0.65, 0.35 };
static const float32_t column2[]  = { 2, 0, 0.20, 0.80 };
static const float32_t column3[]  = { 2, 1, 0.40, 0.60 };
static const float32_t column4[]  = { 2, 1, 0.10, 0.90 };
static const float32_t column5[]  = { 2, 2, 0.20, 0.80 };
static const float32_t column6[]  = { 3, 2, 0.15, 0.30, 0.55 };
static const float32_t column7[]  = { 2, 3, 0.20, 0.70 };
static const float32_t column8[]  = { 3, 4, 0.10, 0.25, 0.65 };
static const float32_t column9[]  = { 4, 4, 0.10, 0.10, 0.20, 0.60 };
static const float32_t column10[] = { 6, 5, 0.10, 0.10, 0.20, 0.60, 0.20, 0.10 };
static const float32_t column11[] = { 6, 6, 0.10, 0.10, 0.20, 0.60, 0.20, 0.10 };
static const float32_t column12[] = { 8, 6, 0.10, 0.10, 0.10, 0.20, 0.20, 0.60, 0.10, 0.10 };
static const float32_t column13[] = { 8, 8, 0.10, 0.10, 0.20, 0.60, 0.20, 0.10, 0.10, 0.10 };
static const float32_t column14[] = { 10, 10, 0.10, 0.10, 0.10, 0.20, 0.20, 0.20, 0.10, 0.10, 0.10, 0.10 };
static const float32_t column15[] = { 10, 12, 0.10, 0.10, 0.10, 0.20, 0.20, 0.20, 0.10, 0.10, 0.10, 0.10 };*/

/*static const float32_t column0[]  = { 1, 0, 1.0 };
static const float32_t column1[]  = { 2, 0, 0.50, 0.50 };
static const float32_t column2[]  = { 3, 0, 0.25, 0.25, 0.50 };
static const float32_t column3[]  = { 3, 1, 0.25, 0.25, 0.50 };
static const float32_t column4[]  = { 3, 2, 0.25, 0.25, 0.50 };
static const float32_t column5[]  = { 4, 2, 0.15, 0.15, 0.30, 0.40 };
static const float32_t column6[]  = { 4, 3, 0.15, 0.15, 0.30, 0.40 };
static const float32_t column7[]  = { 4, 4, 0.15, 0.15, 0.30, 0.40 };
static const float32_t column8[]  = { 5, 4, 0.10, 0.10, 0.20, 0.20, 0.40 };
static const float32_t column9[]  = { 5, 5, 0.10, 0.10, 0.20, 0.20, 0.40 };
static const float32_t column10[] = { 5, 6, 0.10, 0.10, 0.20, 0.20, 0.40 };
static const float32_t column11[] = { 6, 6, 0.05, 0.05, 0.10, 0.10, 0.20, 0.50 };
static const float32_t column12[] = { 6, 7, 0.05, 0.05, 0.10, 0.10, 0.20, 0.50 };
static const float32_t column13[] = { 7, 7, 0.05, 0.05, 0.05, 0.05, 0.10, 0.30, 0.40 };
static const float32_t column14[] = { 7, 8, 0.05, 0.05, 0.05, 0.05, 0.10, 0.30, 0.40 };
static const float32_t column15[] = { 8, 8, 0.05, 0.05, 0.05, 0.05, 0.10, 0.20, 0.20, 0.30 };*/

static const float32_t column0[]  = { 1, 0, 1.0 };
static const float32_t column1[]  = { 1, 1, 1.0 };
static const float32_t column2[]  = { 1, 2, 1.0 };
static const float32_t column3[]  = { 1, 3, 1.0 };
static const float32_t column4[]  = { 1, 4, 1.0 };
static const float32_t column5[]  = { 1, 5, 1.0 };
static const float32_t column6[]  = { 1, 6, 1.0 };
static const float32_t column7[]  = { 1, 7, 1.0 };
static const float32_t column8[]  = { 1, 8, 1.0 };
static const float32_t column9[]  = { 1, 9, 1.0 };
static const float32_t column10[] = { 1, 10, 1.0 };
static const float32_t column11[] = { 1, 11, 1.0 };
static const float32_t column12[] = { 1, 12, 1.0 };
static const float32_t column13[] = { 1, 13, 1.0 };
static const float32_t column14[] = { 1, 14, 1.0 };
static const float32_t column15[] = { 1, 15, 1.0 };

static const float32_t *columnData[] = {
                                        column0, column1, column2, column3,
                                        column4, column5, column6, column7,
                                        column8, column9, column10, column11,
                                        column12, column13, column14, column15
                                    };

static const uint32_t lowLevelColors[5] = { 0xD30DFF, 0x4E0FE8, 0x003AFF, 0x0CAEE8, 0x00FFBC };
static const uint32_t mediumLevelColors[5] = { 0x2CFF0D, 0xBEE80F, 0xFFDC00, 0xE89F0C, 0xFF6C00 };
static const uint32_t highLevelColors[5] = { 0xFF960D, 0xE84B00, 0xFF1400, 0xE80C88, 0xC800FF };

uint32_t columnDivider[16];
uint8_t dotCounter;
uint8_t peak[16];
float32_t columns[16][COLUMN_AVERAGE_FRAMES]; // Column levels for previous 10 frames
float32_t maximumAverageLevel[16]; // Used for dynamically adjusting pseudo rolling averages for prior frames

// Two matrix boards of 8x8, tiled horizontally
Matrix::Matrix()
    : Adafruit_GFX(MATRIX_SIZE * 2, MATRIX_SIZE),
      Adafruit_DotStar(MATRIX_SIZE * 2 * MATRIX_SIZE, MATRIX_DATA_PIN, MATRIX_CLOCK_PIN, DOTSTAR_BRG)
{
}

static uint32_t mix(uint8_t weight, uint32_t startColor, uint32_t endColor) {
    uint32_t startRed = (startColor & 0xFF0000) >> 16;
    uint32_t startGreen = (startColor & 0xFF00) >> 8;
    uint32_t startBlue = (startColor & 0xFF);

    uint32_t endRed = (endColor & 0xFF0000) >> 16;
    uint32_t endGreen = (endColor & 0xFF00) >> 8;
    uint32_t endBlue = (endColor & 0xFF);

    uint32_t mixedRed = floor(startRed * ((255.0f - weight) / 255.0f) + endRed * (weight / 255.0f));
    uint32_t mixedGreen = floor(startGreen * ((255.0f - weight) / 255.0f) + endGreen * (weight / 255.0f));
    uint32_t mixedBlue = floor(startBlue * ((255.0f - weight) / 255.0f) + endBlue * (weight / 255.0f));

    uint32_t color = (
        (mixedRed << 16) +
        (mixedGreen << 8) +
        mixedBlue
    );

    return color;
};

// Expand 16-bit input color (Adafruit_GFX colorspace) to 24-bit (DotStar)
// (w/gamma adjustment)
static uint32_t expandColor(uint16_t color)
{
    return ((uint32_t)pgm_read_byte(&gamma5[color >> 11 ]) << 16) |
           ((uint32_t)pgm_read_byte(&gamma6[(color >> 5) & 0x3F]) << 8) |
           pgm_read_byte(&gamma5[color & 0x1F]);
};

// Downgrade 24-bit color to 16-bit (add reverse gamma lookup here?)
uint16_t Matrix::Color(uint8_t red, uint8_t green, uint8_t blue)
{
    return ((uint16_t)(green & 0xF8) << 8) |
           ((uint16_t)(red & 0xFC) << 3) |
                      (blue >> 3);
};

void Matrix::initialize(AudioVisualizer pVisualizer) {
    visualizer = pVisualizer;
    state = STATE_VISUALIZE;

    begin();
    setTextWrap(false);
    setBrightness(72);
    fillScreen(0);
    show();

    colorIndex = 0;
    colorPosition = 0;
    frameIndex = 0;
    lastTime = 0;
    lastBlink = millis();
    lastStateChange = millis();

    uint8_t i;
    for (i = 0; i < 16; i++) {
        maximumAverageLevel[i] = 1;
    }
}

void Matrix::drawBars() {
    uint32_t highColor = mix(colorPosition, highLevelColors[colorIndex % 5], highLevelColors[(colorIndex + 1) % 5]);
    uint32_t mediumColor = mix(colorPosition, mediumLevelColors[colorIndex % 5], mediumLevelColors[(colorIndex + 1) % 5]);
    uint32_t lowColor = mix(colorPosition, lowLevelColors[colorIndex % 5], lowLevelColors[(colorIndex + 1) % 5]);

    fillRect(0, 0, 16, 3, Matrix::Color((highColor & 0xFF0000) >> 16, (highColor & 0xFF00) >> 8, (highColor & 0xFF)));
    fillRect(0, 3, 16, 2, Matrix::Color((mediumColor & 0xFF0000) >> 16, (mediumColor & 0xFF00) >> 8, (mediumColor & 0xFF)));
    fillRect(0, 5, 16, 3, Matrix::Color((lowColor & 0xFF0000) >> 16, (lowColor & 0xFF00) >> 8, (lowColor & 0xFF)));
};

void Matrix::drawPixel(int16_t x, int16_t y, uint16_t color) {
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

void Matrix::fillScreen(uint16_t color) {
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
            stateDuration = VISUALIZE_DURATION;
            visualize();
            break;
        case STATE_BEER:
            stateDuration = BEER_DURATION;
            animate(beerAnimation, BEER_FRAMES, FRAME_DURATION);
            break;
        case STATE_EYES:
            stateDuration = EYES_DURATION;
            renderEyes();
            break;
        case STATE_TEXT:
            stateDuration = TEXT_DURATION;
            writeText();
            break;
        case STATE_HEART:
            stateDuration = HEART_DURATION;
            drawHearts();
            break;
        case STATE_COLOR_SWIRL:
            stateDuration = COLOR_SWIRL_DURATION;
            animate(colorSwirlAnimation, COLOR_SWIRL_FRAMES, FRAME_DURATION);
            break;
        default:
            visualize();
            break;
    }

    if (millis() - lastStateChange > stateDuration) {
        uint8_t shouldChange = random(max(1, 10000 - (millis() - lastStateChange)));
        if (shouldChange == 0) {
            colorIndex = 0;
            colorPosition = 0;
            frameIndex = 0;
            state = random(0, 255);
            if (state < 80) {
                state = STATE_VISUALIZE;
            } else {
                state = state % TOTAL_STATES;
            }
            lastStateChange = millis();
        }
    }
}

void Matrix::visualize() {
    clear();

    setBrightness(100);
    drawBars();

    float32_t *data;
    float32_t *output = visualizer.getSmoothedOutput();
    float32_t maximum = visualizer.getLastMaximumValue();
    uint8_t i, c, x, y;
    float32_t volume, maximumLevel, level;
    uint8_t numberOfBins;
    uint8_t startBin;

    for (x = 0; x < 16; x++) {
        level = 0;
        volume = 0;
        maximumLevel = 0;

        data = (float32_t *)columnData[x];
        numberOfBins = data[0];
        startBin = data[1];

        for (i = 0; i < numberOfBins; i++) {
            volume += output[startBin + i] * data[i + 2];
        }

        columns[x][frameIndex] = volume;
        maximumLevel = columns[x][0];
        for (i = 0; i < COLUMN_AVERAGE_FRAMES; i++) {
            if (columns[x][i] > maximumLevel)
                maximumLevel = columns[x][i];
        }

        maximumLevel = max(0, (maximum * 5 + maximumLevel) / 6.0f);

        maximumAverageLevel[x] = (maximumAverageLevel[x] * 5 + maximumLevel) / 6.0f;

        level = 10.0f * (columns[x][frameIndex]) / (maximumAverageLevel[x]);

        if (level < 0)       c = 0;
        else if (level > 10) c = 10;
        else                 c = (uint8_t)(floor(level));

        if (c > peak[x]) peak[x] = c;

        if (peak[x] <= 0) {
            drawLine(x, 0, x, 7, Matrix::Color(0, 0, 0));
            continue;
        } else if (c < 8) {
            drawLine(x, 0, x, 7 - c, Matrix::Color(0, 0, 0));
        }

        uint32_t peakColor;

        y = 8 - peak[x];
        if (y < 2) {
            peakColor = mix(colorPosition, highLevelColors[colorIndex % 5], highLevelColors[(colorIndex + 1) % 5]);
            drawPixel(x, y, Matrix::Color((peakColor & 0xFF0000) >> 16, (peakColor & 0xFF00) >> 8, (peakColor & 0xFF)));
        } else if (y < 6) {
            peakColor = mix(colorPosition, mediumLevelColors[colorIndex % 5], mediumLevelColors[(colorIndex + 1) % 5]);
            drawPixel(x, y, Matrix::Color((peakColor & 0xFF0000) >> 16, (peakColor & 0xFF00) >> 8, (peakColor & 0xFF)));
        } else {
            peakColor = mix(colorPosition, lowLevelColors[colorIndex % 5], lowLevelColors[(colorIndex + 1) % 5]);
            drawPixel(x, y, Matrix::Color((peakColor & 0xFF0000) >> 16, (peakColor & 0xFF00) >> 8, (peakColor & 0xFF)));
        }
    }

    show();

    if (++dotCounter >= 1) {
        dotCounter = 0;
        for (x = 0; x < 16; x++) {
            if (peak[x] > 0) peak[x]--;
        }
    }

    if (++frameIndex >= COLUMN_AVERAGE_FRAMES) frameIndex = 0;

    if (++colorPosition == 0) {
        colorIndex++;
    }
}

void Matrix::renderEyes() {
    clear();

    setBrightness(72);

    uint8_t shouldChange = random(eyeDirection == 0 ? 4 : 16);
    if (shouldChange == 0) {
        eyeDirection = random(12) + 1;
    }

    if (millis() - lastBlink > 2000) {
        uint8_t shouldBlink = random(max(1, 2000 - (millis() - lastBlink)));
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
    setBrightness(48);
    frameIndex++;
    if (frameIndex >= numberOfFrames) {
        frameIndex = 0;
    }

    drawPictures(frames, frameIndex);
    show();

    lastTime = millis();
}

void Matrix::drawHearts() {
    clear();

    setBrightness(96);
    uint32_t color, startColor, endColor;
    if (colorIndex % 15 < 5) {
        startColor = highLevelColors[colorIndex % 5];
        endColor = colorIndex % 15 == 4 ? mediumLevelColors[0] : highLevelColors[(colorIndex % 5) + 1];
    } else if (colorIndex % 15 < 10) {
        startColor = mediumLevelColors[colorIndex % 5];
        endColor = colorIndex % 15 == 9 ? lowLevelColors[0] : mediumLevelColors[(colorIndex % 5) + 1];
    } else {
        startColor = lowLevelColors[colorIndex % 5];
        endColor = colorIndex % 15 == 14 ? highLevelColors[0] : lowLevelColors[(colorIndex % 5) + 1];
    }

    color = mix(colorPosition, startColor, endColor);

    drawXBitmap(0, 0, HEART, 8, 8, Matrix::Color((color & 0xFF0000) >> 16, (color & 0xFF00) >> 8, (color & 0xFF)));
    drawXBitmap(8, 0, HEART, 8, 8, Matrix::Color((color & 0xFF0000) >> 16, (color & 0xFF00) >> 8, (color & 0xFF)));

    show();

    colorPosition += 16;
    if (colorPosition == 0) {
        colorIndex++;
    }
}

void Matrix::writeText() {
    if (millis() - lastTime < 32) {
        return;
    }

    lastTime = millis();
    setBrightness(192);
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
