// Arduino gpsFreq Library
// https://github.com/JChristensen/gpsFreq
// Copyright (C) 2013-2022 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// GPS Frequency Counter Library for Arduino
// Uses 1 PPS from a GPS receiver as an accurate time base.
// Tested with Arduino 1.8.19 and an Arduino Uno.
// Connect the 1 PPS signal to INT0 (D2 pin).
// Connect the input frequency to be measured to T1 (D5 pin).

#ifndef GPSFREQ_H_INCLUDED
#define GPSFREQ_H_INCLUDED

#include <Arduino.h>

class FreqCounter
{
    public:
    
        FreqCounter()
        {
            pinMode(LED_BUILTIN, OUTPUT);
        };
        
        void start(uint8_t period);             // gatePeriod in seconds (1, 10, 100)
        void formatFreq(char *result);          // returns freq as a formatted string
        
        volatile bool isBusy = false;           // flag to indicate counting complete
        uint32_t freq = 0;                      // the counted frequency
        volatile uint16_t t1overflow = 0;       // timer1 overflow count
        
        uint8_t gatePeriod = 1;                 // gate period in seconds
        volatile uint8_t gateInterrupts = 0;    // number of interrupts (caused by the 1PPS gate signal)
        //volatile uint16_t ppsTotal = 0;         // cumulative count of pps interrupts
};

extern FreqCounter gpsFreq;

#endif

