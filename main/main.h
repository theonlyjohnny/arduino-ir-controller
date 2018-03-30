#ifndef MAIN_H_LOADED
#define MAIN_H_LOADED

#include <Servo.h>

// SERVO PINS
int ServoLeftPin = 13;
int ServoRightPin = 12;

// IR PINS
int IrDetLeftPin = 10;
int IrLedLeftPin = 9;

int IrDetBackPin = 5;
int IrLedBackPin = 11;

int IrDetRightPin = 3;
int IrLedRightPin = 2;


// RED LEDs
int LedLeftPin = 8;   
int LedRightPin = 7;

// MISC PINS
int BeeperPin = 4;
int IrDetPin = 10;

enum IR_CODES: byte {
    ONE = 128,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    ZERO,
    ENTER = 139,
    CHAN_UP = 144,
    CHAN_DOWN,
    VOL_UP,
    VOL_DOWN,
    MUTE,
    POWER,
    PREVIOUS_CHANNEL = 187
};

// IR DETAILS
long FREQUENCY = 38000;
int DISTANCE_THRESHOLD = 2;

#endif
