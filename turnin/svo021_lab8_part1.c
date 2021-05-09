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
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

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

enum note_states {note_silent, note_c, note_d, note_e} note_state;

unsigned char tempA = 0x00;

void tone_tick() {
    tempA = (~PINA) & 0x07;
    switch(note_state) {
        case note_silent:
            set_PWM(0);
            if (tempA == 0x01) {
                note_state = note_c;
            } 
            else if (tempA == 0x02) {
                note_state = note_d;
            }
            else if (tempA == 0x04) {
                note_state = note_e;
            } 
            else {
                note_state = note_silent;
            }
            break;
        case note_c:
            set_PWM(261.63);
            if (tempA != 0x01) {
                note_state = note_silent;
            }
            else note_state = note_c;
            break;
        case note_d:
            set_PWM(293.66);
            if (tempA != 0x02) {
                note_state = note_silent;
            }
            else note_state = note_d;
            break;
        case note_e:
            set_PWM(329.63);
            if (tempA != 0x04) {
                note_state = note_silent;
            }
            else note_state = note_e;
    }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    PWM_on();
    /* Insert your solution below */
    note_state = note_silent;
    while (1) {
        tone_tick();
    }
    return 1;
}
