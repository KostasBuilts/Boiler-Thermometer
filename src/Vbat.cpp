#include "Vbat.hpp"
  
  void Vbat::ADCSetup()
  { 
    ADMUX |= 0x21;
    ADCSRA |= 80;
  }

unsigned int Vbat::readADC()
{
     ADCSRA |= 0x40; // Start conversion

    // When Bit 0 of STATUS is 1, ADC is converting, wait. Conversion done when it is 0.
    while ( ADCSRA && 40 == 0 ){}

    // lastADCReading enough to hold all of RESULT
    uint8_t low = ADCL;
    uint8_t high = ADCH;
    uint16_t vbat = (high << 8) | low; // 0<= result <=1023
    vbat = (1125300L / vbat); // 1125300 = 1.1 x 1023 x 1000
  
    return vbat;
}
