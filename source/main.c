/*	Author: lab
 *  Partner(s) Name: Scott Vo
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
    TCCR1B = 0x0B;

    OCR1A = 125;

    TIMSK1 = 0x02;

    TCNT1 = 0;

    _avr_timer_cntcurr = _avr_timer_M;

    SREG |= 0x80;
}

void TimerOff() {
    TCCR1B = 0x00;
}

void TimerISR() {
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
    _avr_timer_cntcurr--;
    if(_avr_timer_cntcurr == 0) {
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

void set_PWM(double frequency) {
    static double current_frequency;

    if (frequency != current_frequency) {
        if (!frequency) {TCCR3B &= 0x08;}
        else {TCCR3B |= 0x03;}

        if (frequency < 0.954) {OCR3A = 0xFFFF; }

        else if (frequency > 31250) { OCR3A = 0x0000; }

        else {OCR3A = (short)(8000000 / (128 * frequency)) - 1;}

        TCNT3 = 0;
        current_frequency = frequency;
    }
}

void PWM_on() {
    TCCR3A = (1 << COM3A0);

    TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);

    set_PWM(0);
}

void PWM_off() {
    TCCR3A = 0x00;
    TCCR3B = 0x00;
}

double c_scale[10] = {261.63, 277.18, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25, 0};
unsigned char sequence[20] = {9, 9, 9, 1, 2, 4, 6, 8, 7, 5, 2, 4, 3, 5, 7, 8, 8, 9, 9, 9};
/*
C:0
C#:1
D:2
E:3
F:4
G:5
A:6
B:7
C:8
none: 9
*/
enum mel_states {mel_wait, mel_play} mel_state;

unsigned char current_degree = 0x00;
unsigned char tempA = 0x00;
unsigned char play_flag = 0x00;

void mel_tick() {
    switch(mel_state) {
        case mel_wait:
            current_degree = 0x00;
            if (play_flag == 0x00) mel_state = mel_wait;
            else mel_state = mel_play;
            break;
        case mel_play:
            set_PWM(c_scale[sequence[current_degree]]);
            if((current_degree < 19) && (play_flag & 0x01) == 0x01) {
                current_degree++;
                mel_state = mel_play;
            }
            else {
                mel_state = mel_wait;
                play_flag = 0x00;
            }
    }

}

enum play_states {play_wait, play_press} play_state;

void play_tick() {
    tempA = (~PINA) & 0x01;
    switch(play_state){
        case play_wait:
            if(tempA == 0x01 && play_flag == 0x00) {
                play_state = play_press;
                play_flag = 0x01;
            }
            else play_state = play_wait;
            break;
        case play_press: 
            if (tempA == 0x01) {
                play_state = play_press;
            }
            else play_state = play_wait;
            break;
    }

}


int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    TimerSet(250);
    TimerOn();
    PWM_on();
    mel_state = mel_wait;
    play_state = play_wait;
    /* Insert your solution below */
    while (1) {
        while(!TimerFlag);
        mel_tick();
        play_tick();
        TimerFlag = 0;
    }
    return 1;
}
