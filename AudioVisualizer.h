#ifndef _AUDIO_VISUALIZER_H_
#define _AUDIO_VISUALIZER_H_

#define ARM_MATH_CM0
#include <Arduino.h>
#include <arm_math.h>
#include <arm_const_structs.h>

#include "constants.h"

void disableADC();
void initADC();
void resetADC();
void window(float32_t *samples);

class AudioVisualizer {
public:
    AudioVisualizer();

    void initialize();
    void loop();
    float32_t getDB(float32_t sample);
    float32_t getAverageValue();
    uint32_t getMaximumIndex();
    float32_t getMaximumValue();
    float32_t* getOutput();
    float32_t* getSmoothedOutput();
};

#endif
