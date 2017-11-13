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

#include "Matrix.h"
#include "Strip.h"
#include "graphics.h"


#define ADC_CHANNEL 0x21 // Connect microphone to D10/ADC13 pin on Flora

// GRAPHICS
int pos = 0;
bool backwards = false;

Matrix matrix = Matrix();
Strip strip = Strip();

void initializeMicrophone() {
    /*ADMUX  = ((1<<REFS1) | (1<<REFS0) | (0<<ADLAR) | (0<<MUX4) | (0<<MUX3) | (0<<MUX2) | (0<<MUX1) | (1<<MUX0));
    ADCSRB &= ((~(1<<ADTS3)) | (~(1<<ADTS2)) | (~(1<<ADTS1)) | (~(1<<ADTS0)));
    ADCSRB |= (1<<MUX5);
    //DIDR2 = 1 << (ADC_CHANNEL - 8);
    ADCSRA = _BV(ADEN)  | // ADC Enable
             _BV(ADSC)  | // ADC Start
             _BV(ADATE) | // Auto trigger
             _BV(ADIE)  | // Interrupt enable
             _BV(ADPS1) | _BV(ADPS0); // 64:1 / 13 = 9615 Hz
    TIMSK0 = 0; // Timer0 off
    sei(); // Enable interrupts*/
}

void setup() {
    matrix.initialize();
    strip.initialize();

    //initializeMicrophone();
}

void loop() {
    matrix.loop();
    strip.loop();
}
