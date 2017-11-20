/**
 *  Sampling frequency              ~14.4kHz
 *  Maximum frequency detectable    ~7.2kHz
 *  Frequency per bin               ~56Hz
 */

#include <math.h>

#include "AudioVisualizer.h"

#define WAIT_ADC_SYNC   while (ADC->STATUS.bit.SYNCBUSY) {}
#define WAIT_ADC_RESET  while (ADC->CTRLA.bit.SWRST) {}

#define ADC_CHANNEL             0x00
#define SMOOTHING               0.75
#define MICROPHONE_LOW          310
#define MICROPHONE_MIDPOINT     1551
#define MICROPHONE_HIGH         2793

float32_t samples[FFT_SAMPLES * 2];
float32_t fftOutput[FFT_SAMPLES];
float32_t fftSmoothed[FFT_SAMPLES / 2];
float32_t windowOutput[FFT_SAMPLES];
volatile bool sampling = false;
volatile int samplePosition = 0;
float32_t maximumValue;
uint32_t maximumIndex;
float32_t averageValue;

void serialDebugFFT() {
    for (int i = 0; i < 32; i++) {
        Serial.print(fftOutput[i]);
        Serial.print("\t");
    }

    Serial.print("\n");
}

void processingDebugFFT() {
    Serial.write(255);
    for (int i = 0; i < FFT_SAMPLES; i++) {
        Serial.write((byte)(round((fftOutput[i] / maximumValue) * 254)));
    }
}

AudioVisualizer::AudioVisualizer() {
    for (int i = 0; i < FFT_SAMPLES; i++) {
        windowOutput[i] = 0.5 - (0.5 * arm_cos_f32((2.0 * PI * i) / (FFT_SAMPLES / 2 - 1)));
    }
}

/**
 * Initialize the Timer clock so that we can take samples
 */
void AudioVisualizer::initialize() {
    // Make sure to enable the ADC clock in power management
    PM->APBCMASK.reg |= PM_APBCMASK_ADC;

    // Enable the generic clock for ADC
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_ADC);
    while (GCLK->STATUS.bit.SYNCBUSY);

    resetADC();
    initADC();
}

float32_t AudioVisualizer::getDB(float32_t sample) {
    return 20 * log10(abs(sample));
}

float32_t AudioVisualizer::getAverageValue() {
    return averageValue;
}

uint32_t AudioVisualizer::getMaximumIndex() {
    return maximumIndex;
}

float32_t AudioVisualizer::getMaximumValue() {
    return maximumValue;
}

float32_t* AudioVisualizer::getOutput() {
    return fftOutput;
}

float32_t* AudioVisualizer::getSmoothedOutput() {
    return fftSmoothed;
}

void AudioVisualizer::loop() {
    if (sampling) {
        return;
    }

    window(samples);
    arm_cfft_f32(&arm_cfft_sR_f32_len128, samples, 0, 1);
    arm_cmplx_mag_squared_f32(samples, fftOutput, FFT_SAMPLES);
    for (int i = 0; i < FFT_SAMPLES / 2; i++) {
        fftOutput[i] = 20 * log10(fftOutput[i]);    // Convert to dB scale
        fftSmoothed[i] = max(fftOutput[i], SMOOTHING * fftSmoothed[i] + ((1 - SMOOTHING) * fftOutput[i]));
    }
    arm_max_f32(fftOutput, FFT_SAMPLES, &maximumValue, &maximumIndex);
    arm_mean_f32(fftOutput, FFT_SAMPLES, &averageValue);
    //serialDebugFFT();

    sampling = true;
    samplePosition = 0;
    NVIC_EnableIRQ(ADC_IRQn);
}

void disableADC() {
    ADC->CTRLA.bit.ENABLE = 0;
    WAIT_ADC_SYNC;

    NVIC_ClearPendingIRQ(ADC_IRQn);
    NVIC_DisableIRQ(ADC_IRQn);
}

void initADC() {
    sampling = true;
    samplePosition = 0;

    ADC->CTRLA.bit.ENABLE = 0;          // Disable ADC
    WAIT_ADC_SYNC;

    // Set external voltage reference to AREF pin
    ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_AREFA_Val;
    WAIT_ADC_SYNC;

    // Set the clock prescaler (48MHz / 256 / 13(cycles per conversion) = ~14.4kHz)
    // Set 12bit resolution
    // Set free running mode (a new conversion will begin as a previous one completes)
    ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV256 | ADC_CTRLB_RESSEL_12BIT | ADC_CTRLB_FREERUN;
    WAIT_ADC_SYNC;

    // Enable Result Ready Interrupt
    ADC->INTENSET.bit.RESRDY = 1;
    WAIT_ADC_SYNC;

    // Set input to read from ADC_CHANNEL and Ground
    ADC->INPUTCTRL.reg = ADC_CHANNEL | ADC_INPUTCTRL_MUXNEG_GND | ADC_INPUTCTRL_GAIN_1X;
    WAIT_ADC_SYNC;

    ADC->CTRLA.bit.ENABLE = 1;
    WAIT_ADC_SYNC;

    NVIC_EnableIRQ(ADC_IRQn);
}

void resetADC() {
    WAIT_ADC_SYNC;

    ADC->CTRLA.bit.ENABLE = 0;          // Disable ADC
    WAIT_ADC_SYNC;

    ADC->CTRLA.bit.SWRST = 1;           // Reset ADC
    WAIT_ADC_SYNC;
    WAIT_ADC_RESET;
}

void window(float32_t *samples) {
    for (int i = 0; i < FFT_SAMPLES; i += 2) {
        samples[i] *= windowOutput[i / 2];
    }
}

void ADC_Handler(void) {
    if (!sampling || samplePosition >= FFT_SAMPLES) {
        ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;
        WAIT_ADC_SYNC;

        return;
    }

    // Read the value from the result register, then map the resulting value to the
    // expected microphone values
    // Microphone has DC bias of 1.25V and 2Vpp. VCC is 3.3V, reading is 12b (so 0-4095)
    float32_t value = (float32_t)ADC->RESULT.reg;
    value = (value - MICROPHONE_LOW) * (2) / (MICROPHONE_HIGH - MICROPHONE_LOW) - 1;

    samples[samplePosition * 2] = value;
    // Odd values are complex, set to 0
    samples[samplePosition * 2 + 1] = 0;

    if (++samplePosition >= FFT_SAMPLES) {
        sampling = false;
        NVIC_ClearPendingIRQ(ADC_IRQn);
        NVIC_DisableIRQ(ADC_IRQn);
    }

    ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;
    WAIT_ADC_SYNC;
}
