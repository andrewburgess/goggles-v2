#ifndef _AUDIO_VISUALIZER_H_
#define _AUDIO_VISUALIZER_H_

#define ARM_MATH_CM0
#include <Arduino.h>
#include <arm_math.h>
#include <arm_const_structs.h>

void disableADC();
void initADC();
void resetADC();

class AudioVisualizer {
public:
    AudioVisualizer();

    void initialize();
    void loop();
    float32_t* getOutput();
    int32_t getSampleCount();
};

#endif
