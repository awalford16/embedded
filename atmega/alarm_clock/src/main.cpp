#include <Arduino.h>
#include "qpn.h"
#include "ClockAlarm_SM.h"
#include "lcd.h"
#include "main.h"

Q_DEFINE_THIS_FILE;

static void Timer1_setup(void);

void setup()
{
    Timer1_setup();
    Clock_Alarm_ctor();
}

void loop()
{
}

static void Timer1_setup(void)
{
    TCCR1A = 0;          // CTC mode
    TCCR1B = B00001100;  // prescaler to 256
    TIMSK1 |= B00000010; // Interrupt enable for OCR1A compare match
    OCR1A = 6250 - 1;    // OC Match value for 100ms time base generation
}