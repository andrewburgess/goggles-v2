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

#define ADC_CHANNEL 0x22 // Connect microphone to D6/ADC10 pin on Flora

// Fast Hartley Transform defines
#define FHT_N 64

#include <FHT.h>

volatile byte samplePosition = 0;

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

void initializeMicrophone() {
#if (ADC_CHANNEL > 7)
    ADMUX = _BV(REFS0) | (ADC_CHANNEL - 8);
    ADCSRB = _BV(MUX5); // Free run mode, high MUX bit
    DIDR2 = 1 << (ADC_CHANNEL - 8);
#else
    ADMUX = _BV(REFS0) | ADC_CHANNEL;
    ADCSRB = 0; // Free run mode, no high MUX bit
    DIDR0 = 1 << ADC_CHANNEL;
#endif

    ADCSRC = _BV(ADEN)  | // ADC Enable
             _BV(ADSC)  | // ADC Start
             _BV(ADATE) | // Auto trigger
             _BV(ADIE)  | // Interrupt enable
             _BV(ADPS1) | _BV(ADPS0); // 64:1 / 13 = 9615 Hz
    TIMSK0 = 0; // Timer0 off
    sei(); // Enable interrupts
}

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
    Serial.begin(115200);
    setupMatrix();
    setupStrip();

    initializeMicrophone();
}

void loop() {
    matrix.drawXBitmap(0, 0, heart, 8, 8, 0xFF0000);
    matrix.show();

    while(ADCSRA & _BV(ADIE)); //Wait for audio sampling to finish
    fht_window();
    fht_reorder();
    fht_run();
    fht_mag_log();

    Serial.write(255);
    Serial.write(fht_log_out, FHT_N / 2);
}

ISR(ADC_vect) { // Audio-sampling interrupt
    static const int16_t noiseThreshold = 4;
    int16_t sample = ADC;
    fht_input[samplePosition] = ((sample > (512 - noiseThreshold)) && (sample < (512 + noiseThreshold))) ? 0 : sample - 512;

    if (++samplePosition >= FHT_N) ADCSRA &= ~_BV(ADIE); // Buffer full, turn interrupt off
}
