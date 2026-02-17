// Arduino gpsFreq Library
// https://github.com/JChristensen/gpsFreq
// Copyright (C) 2013-2022 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// GPS Frequency Counter Library for Arduino
// Uses 1 PPS from a GPS receiver as an accurate time base.
// Tested with Arduino 1.8.19 and an Arduino Uno.
// Connect the input frequency to be measured to T1 (D5 pin).
// Connect the 1 PPS signal to INT0 (D2 pin).

#include <gpsFreq.h>

// global object
FreqCounter gpsFreq;

void FreqCounter::start(uint8_t period)
{
    isBusy = true;
    
    gatePeriod = period;
    gateInterrupts = 0;

    // v 1.1.0
    TIMSK0 &= ~_BV(TOIE0);  // disable timer 0 overflow interrupt -- disables millis(), delay()
    
    EICRA = _BV(ISC01);     // external interrupt on falling edge
    EIFR = _BV(INTF0);      // clear the interrupt flag (setting ISCnn can cause an interrupt)
    EIMSK = _BV(INT0);      // enable external interrupt
}

ISR(INT0_vect)
{
    // stop counting
    
    if (gpsFreq.gateInterrupts >= gpsFreq.gatePeriod)
    {
        TCCR1B = 0;                     // stop timer 1
        TIMSK1 = 0;                     // stop timer 1 overflow interrupt
        EIMSK = 0;                      // stop external interrupt
        
        // v 1.1.0
        TIMSK0 |= _BV(TOIE0);           // enable timer 0 overflow interrupt
        
        gpsFreq.freq = ((uint32_t) gpsFreq.t1overflow << 16) + TCNT1;
        
        gpsFreq.isBusy = false;
        
        //digitalWrite(LED_BUILTIN, LOW);
    }
    
    // start counting
    
    else if (gpsFreq.gateInterrupts == 0)
    {   
        TCCR1B = 0;
        TCCR1A = 0;                     // stop timer 1
        TCCR1C = 0;
        TIMSK1 = 0;
        TCNT1 = 0;                      // zero timer 1
        TIFR1 = _BV(TOV1);              // clear timer 1 overflow flag
        gpsFreq.t1overflow = 0;
        
        TIMSK1 = _BV(TOIE1);            // interrupt on timer 1 overflow
        TCCR1B = _BV(CS12) | _BV(CS11); // start timer 1, external clock on falling edge
        
        //digitalWrite(LED_BUILTIN, HIGH);
    }
    
    ++gpsFreq.gateInterrupts;
    //++gpsFreq.ppsTotal;
}

ISR(TIMER1_OVF_vect)
{
    ++gpsFreq.t1overflow;
}

void FreqCounter::formatFreq(char *result)
{
    // return the frequency as a formatted string
    
    char f[16];

    ltoa(freq / gatePeriod, f, 10);
    char *pf = f;
    uint8_t len = strlen(f);
    
    for (uint8_t i = 0; i < len; ++i)
    {
        *result++ = *pf++;
        
        if ((len - i - 1) % 3 == 0 && i < len-1)
            *result++ = ' ';
    }

    if (gatePeriod > 1)
    {
        itoa(freq % gatePeriod, f, 10);
        *result++ = '.';
        
        if (strlen(f) < 2 && gatePeriod > 10)
            *result++ = '0';
        
        pf = f;
        
        while ((*result++ = *pf++));
    }
    else
    {
        *result++ = 0;
    }
}

