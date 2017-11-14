#include "AudioVisualizer.h"

#define WAIT_ADC_SYNC   while (ADC->STATUS.bit.SYNCBUSY) {}
#define WAIT_ADC_RESET  while (ADC->CTRLA.bit.SWRST) {}

#define ADC_CHANNEL     0x00
#define FFT_SAMPLES     256

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

    NVIC_EnableIRQ(ADC_IRQn);

    resetADC();
    initADC();
}

void AudioVisualizer::initADC() {
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
    ADC->INPUTCTRL.reg = ADC_CHANNEL | ADC_INPUTCTRL_MUXNEG_IOGND | ADC_INPUTCTRL_GAIN_1X;
    WAIT_ADC_SYNC;

    ADC->CTRLA.bit.ENABLE = 1;
    WAIT_ADC_SYNC;
}

void AudioVisualizer::onADCReady() {
    uint32_t value = ADC->RESULT.reg;
    Serial.println(value);

    ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;
    WAIT_ADC_SYNC;
}

void AudioVisualizer::resetADC() {
    WAIT_ADC_SYNC;

    ADC->CTRLA.bit.ENABLE = 0;          // Disable ADC
    WAIT_ADC_SYNC;

    ADC->CTRLA.bit.SWRST = 1;           // Reset ADC
    WAIT_ADC_SYNC;
    WAIT_ADC_RESET;
}

void ADC_Handler(void) {
    
}
