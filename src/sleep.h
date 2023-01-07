#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#define Watchdog_Timer      9

class SleepClass
{
public:
    SleepClass();
    SleepClass(int WDTV);
    void system_sleep();

private:
    void _setup_watchdog(int WDT);
    int _WDTV = Watchdog_Timer;
};
