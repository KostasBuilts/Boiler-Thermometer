#include <sleep.h>

SleepClass::SleepClass(int WDTV)
{
    _WDTV = WDTV;
}

SleepClass::SleepClass()
{
    //Nothing
}

void SleepClass::_setup_watchdog(int WDT)
{
  // 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
  // 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec

  uint8_t bb;
  if (WDT > 9 ) WDT = 9;
  bb = WDT & 7;
  if (WDT > 7) bb |= (1 << 5);
  bb |= (1 << WDCE);

  MCUSR &= ~(1 << WDRF);
  // start timed sequence
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  // set new watchdog timeout value
  WDTCSR = bb;
  WDTCSR |= _BV(WDIE);
}


// system wakes up when watchdog is timed out
void SleepClass::system_sleep()
{
  ADCSRA |= (0<<ADEN);                   // switch Analog to Digitalconverter OFF
  _setup_watchdog(_WDTV);                   // max of 8 seconds sleep

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  sei();                               // Enable the Interrupts so the wdt can wake us up

  sleep_mode();                        // System sleeps here

  sleep_disable();                     // System continues execution here when watchdog timed out
  ADCSRA |= (1<<ADEN);                   // switch Analog to Digitalconverter ON
}
