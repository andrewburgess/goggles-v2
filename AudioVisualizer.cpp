#include "AudioVisualizer.h"

#define WAIT_ADC_SYNC   while (ADC->STATUS.bit.SYNCBUSY) {}
#define WAIT_ADC_RESET  while (ADC->CTRLA.bit.SWRST) {}

#define ADC_CHANNEL     0x00
#define FFT_SAMPLES     256
#define NOISE_THRESHOLD 12

float32_t samples[FFT_SAMPLES];
float32_t fftOutput[FFT_SAMPLES / 2];
volatile bool sampling = false;
volatile int samplePosition = 0;
float32_t maxValue;
uint32_t testIndex;

void serialDebugFFT() {
    for (int i = 0; i < FFT_SAMPLES/2; i++) {
        Serial.print(fftOutput[i]);
        Serial.print("\t");
    }

    Serial.print("\n");
}

void processingDebugFFT() {
    Serial.write(255);

    for (int i = 0; i < FFT_SAMPLES/2; i++) {
        Serial.write(fftOutput[i]);
    }
}

AudioVisualizer::AudioVisualizer() {

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

float32_t* AudioVisualizer::getOutput() {
    return fftOutput;
}

int32_t AudioVisualizer::getSampleCount() {
    return FFT_SAMPLES / 2;
}

void AudioVisualizer::loop() {
    if (sampling) {
        return;
    }

    maxValue = 0;

    arm_cfft_f32(&arm_cfft_sR_f32_len128, samples, 0, 1);
    arm_cmplx_mag_f32(samples, fftOutput, FFT_SAMPLES / 2);
    arm_max_f32(fftOutput, FFT_SAMPLES, &maxValue, &testIndex);

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

    // Set the clock prescaler (48MHz / 64 / 13(cycles per conversion) = ~57kHz)
    // Set 12bit resolution
    // Set free running mode (a new conversion will begin as a previous one completes)
    ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV64 | ADC_CTRLB_RESSEL_12BIT | ADC_CTRLB_FREERUN;
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

void ADC_Handler(void) {
    if (!sampling || samplePosition >= FFT_SAMPLES) {
        ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;
        WAIT_ADC_SYNC;

        return;
    }

    // Read the value from the result register, then subtract 1.56V from the
    // reading
    float32_t value = (float32_t)ADC->RESULT.reg;
    value = value - 1560;

    samples[samplePosition] = (float32_t)value;
    // Odd values are complex, set to 0
    samples[++samplePosition] = 0;

    if (++samplePosition >= FFT_SAMPLES) {
        sampling = false;
        NVIC_ClearPendingIRQ(ADC_IRQn);
        NVIC_DisableIRQ(ADC_IRQn);
    }

    ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;
    WAIT_ADC_SYNC;
}
