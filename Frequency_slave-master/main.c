/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usitwislave.h"
#include "usitwislave_devices.h"
#include "usitwislave.c"

#define DEVICE_ID 0x03
#define TIMER_OVERFLOW_COUNT 1 //https://electronics.stackexchange.com/questions/74483/calculate-exact-timer-frequency-for-attiny

//Timer 1 - timer

volatile int timerCount = 0; //https://arduino.stackexchange.com/questions/20994/why-the-need-to-use-the-volatile-keyword-on-global-variables-when-handling-inter
volatile int ticks = 0;
int counterReady = 1;
uint16_t frq = 0;
int period = 500;
int state;
uint8_t sendhigh = 0;
uint8_t sendlow = 0;

void disable_interrupt()
{
    GIMSK &= ~(1 << PCIE);
}

void enable_interrupt()
{
    GIMSK |= (1 << PCIE);
}

void startCounting ()//https://electronics.stackexchange.com/questions/255067/timer1-on-attiny85-not-working-as-expected/255109
{
    
    TCCR1 = 0;
    OCR1C = 255; 
    
    TIMSK = 1 << TOIE1;
    
    PCMSK |= (1 << PCINT1);
    GIMSK |= (1 << PCIE);
    
    // Reset prescalers
    GTCCR |= (1<<PSR1);
    sei();
    // start Timer 1
    TCCR1 = (1<<CS10) | (1<<CS11) | (1<<CS12) | (1<<CS13) ;  // prescaler of 16384
}  // end of startCounting

ISR(TIMER1_OVF_vect)
{
    timerCount++;
}

ISR(PCINT0_vect)
{
    ticks++;
}

static void twi_callback(uint8_t input_buffer_length, const uint8_t *input_buffer,
                         uint8_t *output_buffer_length, uint8_t *output_buffer)
{
    if(input_buffer_length > 0){ //we're getting an instruction from master
        state = 2;
    }else{ // we're getting a write request from master
        *output_buffer_length = 8;
        switch(state)
        {
            case(2):{ // sending device id
                *output_buffer = sendhigh;
                state = 1;
                break;
            }
            case(1):{ // sending device id
                *output_buffer = sendlow;
                state = 0;
                break;
            }
            case(0):{ // sending device id
                *output_buffer = DEVICE_ID;
                break;
            }
        }
    }
}


int main(void)
{
    startCounting();
    
    while (counterReady == 1)
    {
        disable_interrupt();
        if( timerCount >= TIMER_OVERFLOW_COUNT)
        {
            TCCR1 = 0;    // stop timer 1
            TIMSK = 0;    // disable Interrupt
            counterReady = 0;
        }
        enable_interrupt();
    }  // loop until count over
    disable_interrupt();

    frq = ticks;
    sendhigh = (uint8_t)(frq >> 8);
    frq = ticks;
    sendlow = (uint8_t)(frq);
    
    usi_twi_slave(DEVICE_ID, 1, *twi_callback, 0);
    return 0;

}
