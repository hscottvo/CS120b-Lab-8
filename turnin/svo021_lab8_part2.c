/*	Author: lab
 *  Partner(s) Name: Scott Vo
 *	Lab Section: 021
 *	Assignment: Lab 8  Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *  Demo Link: https://youtu.be/qJ_Ezu4icxs
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

enum pitch_states {pitch_wait, pitch_up, pitch_down} pitch_state;
double c_scale[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
unsigned char current_degree = 0x00;
unsigned char play_flag = 0x00;

void pitch_tick() {
    tempA = (~PINA) & 0x07;
    switch(pitch_state){
        case pitch_wait: 
            if (tempA == 0x02) {
                pitch_state = pitch_up;
                // current_degree = (current_degree >= 7)? current_degree: current_degree + 1;
                if (current_degree < 7){
                    ++current_degree;
                }
                if (play_flag == 0x01) set_PWM(c_scale[current_degree]);
            }
            else if (tempA == 0x04) {
                pitch_state = pitch_down;
                // current_degree = (current_degree <= 0)? current_degree: current_degree - 1;
                if (current_degree > 0) {
                    --current_degree;
                }
                if (play_flag == 0x01) set_PWM(c_scale[current_degree]);
            }
            else {
                pitch_state = pitch_wait;
            }
            break;
        case pitch_up:
            if (tempA == 0x02) {
                pitch_state = pitch_up;
            } 
            else {
                pitch_state = pitch_wait;
            }
            break;
        case pitch_down:
            if (tempA == 0x04) {
                pitch_state = pitch_down;
            }
            else {
                pitch_state = pitch_wait;
            }
            break;        
    }
}

enum tone_states {tone_wait, tone_play, tone_play_wait, tone_play_wait_2} tone_state;

void tone_tick(){
    tempA = (~PINA) & 0x07;
    switch(tone_state){
        case tone_wait: 
            if (tempA == 0x01) {
                tone_state = tone_play;
                set_PWM(c_scale[current_degree]);
                play_flag = 0x01;
            }
            else {
                tone_state = tone_wait;                
                set_PWM(0);   
                play_flag = 0x00;;             
            }
            break;
        case tone_play: 
            if (tempA == 0x00) {
                tone_state = tone_play_wait;
            } else tone_state = tone_play;
            break;
        case tone_play_wait: 
            if (tempA == 0x01) {
                tone_state = tone_play_wait_2;
            } else tone_state = tone_play_wait;
            break;
        case tone_play_wait_2:
            if (tempA == 0x00) {
                tone_state = tone_wait;
            } else tone_state = tone_play_wait_2;
            break;
    }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    PWM_on();
    /* Insert your solution below */
    pitch_state = pitch_wait;
    while (1) {
        pitch_tick();
        tone_tick();
    }
    return 1;
}
