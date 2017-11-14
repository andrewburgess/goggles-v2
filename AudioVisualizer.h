#ifndef _AUDIO_VISUALIZER_H_
#define _AUDIO_VISUALIZER_H_

#define ARM_MATH_CM0
#include <Arduino.h>
#include <arm_math.h>

typedef void (*ADC_CALLBACK) (void);

class AudioVisualizer {
public:
    AudioVisualizer();

    void initialize();

private:
    void initADC();
    void onADCReady();
    void resetADC();
};

#endif
