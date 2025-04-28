#include "Adc.h"

void enableFastAdc() {
  // Set ADC prescale to 16
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  cbi(ADCSRA, ADPS0);
}