#ifndef ANAIN_PRJ_H
#define ANAIN_PRJ_H

#define NUM_SAMPLES 12

#if defined(STM32F4)
#define SAMPLE_TIME ADC_SMPR_SMP_15CYC
#else
#define SAMPLE_TIME ADC_SMPR_SMP_7DOT5CYC
#endif

#define ANA_IN_LIST \
   ANA_IN_ENTRY(throttle, GPIOC, 0) \
   ANA_IN_ENTRY(dummy, GPIOC, 11)

#endif
