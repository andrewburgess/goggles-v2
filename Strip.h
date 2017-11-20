#ifndef _STRIP_H_
#define _STRIP_H_

#include <QList.h>
#include "QList.cpp"
#include <Adafruit_DotStar.h>

#include "AudioVisualizer.h"

#define LED_STRIP_PIXELS    16
#define LED_STRIP_DATA_PIN  6
#define LED_STRIP_CLOCK_PIN 5

class Strip : public Adafruit_DotStar {
public:
    Strip();

    uint16_t colorWheel(byte position);
    void initialize(AudioVisualizer pVisualizer);
    void loop();

    static uint32_t Color(uint8_t red, uint8_t green, uint8_t blue);

private:
    AudioVisualizer visualizer;
    uint8_t brightness;
    QList<float32_t> previousReads;
    long lastTime;
    uint8_t position;
    int head;
    int tail;

    void chase();
};

#endif
