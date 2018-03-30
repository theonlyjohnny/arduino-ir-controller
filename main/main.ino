#include "main.h"

Servo servoLeft;
Servo servoRight;

enum ALERT_DIRECTIONS {
    LEFT,
    RIGHT,
    ALL
};
//constants

int irPulse;
int remoteCode;

void setup() {
    Serial.begin(9600);
    tone(BeeperPin, 3000, 1000);
    delay(1000);
    Serial.println("Hello globe!");
    /* Connect to Serial port and play sound to verify setup loop */

    pinMode(LedLeftPin, OUTPUT);
    pinMode(LedRightPin, OUTPUT);
    /* Register LEDs */


    Serial.print("ONE - ");
    Serial.println(ONE);
    Serial.print("TWO - ");
    Serial.println(TWO);
    Serial.print("THREE - ");
    Serial.println(THREE);
    Serial.print("FOUR - ");
    Serial.println(FOUR);
    Serial.print("FIVE - ");
    Serial.println(FIVE);
    Serial.print("SIX - ");
    Serial.println(SIX);
    Serial.print("SEVEN - ");
    Serial.println(SEVEN);
    Serial.print("EIGHT - ");
    Serial.println(EIGHT);
    Serial.print("NINE - ");
    Serial.println(NINE);
    Serial.print("ZERO - ");
    Serial.println(ZERO);
    /* DEBUG: print IR codes for various buttons */
}

void loop() {
    int remoteCode = GetIrCode();

    if (remoteCode == -1) {
        /* Timeout/no signal */
        if ((servoLeft.attached()==true) && (servoRight.attached()==true)) {
            /* dont need to be attached, so dont be :/ */
            servoLeft.detach();
            servoRight.detach();
        }
    } else {
        if ((servoLeft.attached() == false) && (servoRight.attached() == false)) {
            /* need to be attached, do plz be :D */
            servoLeft.attach(ServoLeftPin);
            servoRight.attach(ServoRightPin);
        }
    }
    delay(10); 

    bool irl = !irDetect(IrLedLeftPin, IrDetLeftPin, FREQUENCY);
    delay(1);
    bool irr = !irDetect(IrLedRightPin, IrDetRightPin, FREQUENCY);
    delay(1);
    bool irb = !irDetect(IrLedBackPin, IrDetBackPin, FREQUENCY);
    int distanceL = irDistance(IrLedLeftPin, IrDetLeftPin);
    delay(1);
    int distanceR = irDistance(IrLedRightPin, IrDetRightPin);
    delay(1);
    int distanceB = irDistance(IrLedBackPin, IrDetBackPin);

    /* irl irr irb  are all measures of whether or not there is an object in front of the ir sensor (irl = ir left, irr = ir right, irb = ir back) */
    /* distanceL distanceR distanceB are all measures of distance between an object that is present and the ir sensor. scale is 1 - 5 */

    if (irb && distanceB <= DISTANCE_THRESHOLD) {
        /* There's something behind us */
        Serial.print("irb triggered. distanceB: ");
        Serial.println(distanceB);
        alert_all();
        return;
    }

    if (irr && distanceR <= DISTANCE_THRESHOLD) {
        /* There's something to the right that will prevent us from moving  */
        Serial.print("irr triggered. distanceR: ");
        Serial.println(distanceR);
        alert_right();
        return;
    }

    if (irr && distanceL <= DISTANCE_THRESHOLD) {
        /* There's something to the left that will prevent us from moving */
        Serial.print("irl triggered. distanceL: ");
        Serial.println(distanceL);
        alert_left();
        return;
    }

    switch(remoteCode) {
        case ONE:
        case VOL_DOWN: //left turn
            move(-200, 0);
            break;
        case THREE:
        case VOL_UP: //right turn
            move(0, -200);
            break;
        case TWO:
        case CHAN_UP: //forwards
            move(-200, -200);
            break;
        case FOUR: // left pivot
            move(-200, 200);
            break;
        case FIVE: //do nothing
            move(0, 0, -1);
            break;
        case SIX: // right pivot
            move(200, -200);
            break;
        case SEVEN: //back left turn
            move(0, -200);
            break;
        case NINE: //back right turn
            move(-200, 0);
            break;
        case EIGHT:
        case CHAN_DOWN: //backwards
            move(200, 200);
            break;
        case MUTE: //test speaker and lights
            alert_all();
            break;
        case ZERO: //debugger
            /* TODO: make this a lock? */
            Serial.println("irl | irr | irb | remoteCode || distanceR | distanceL");

            Serial.print(irl);
            Serial.print(" | ");
            Serial.print(irr);
            Serial.print(" | ");
            Serial.print(irb);
            Serial.print(" | ");
            Serial.print(remoteCode);
            Serial.print(" || ");
            Serial.print(distanceR);
            Serial.print(" | ");
            Serial.print(distanceL);
            Serial.print(" | ");
            Serial.println(distanceB);
            alert_all();
            break;
        case -1: //nohting
            break;
        case 0:
        case 2112:
        case 72:
        case 512:
            Serial.println("Got 0 or 2112 or 72? Tf is this?");
            break;
        default:
            Serial.print("Unmatched remoteCode -- ");
            Serial.println(remoteCode);
            /* move(0, 0, -1); */
            /* servoLeft.detach(); */
            /* servoRight.detach(); */
            break;
    }
}

bool irDetect(int irLedPin, int irReceiverPin, long frequency) {
    tone(irLedPin, frequency, 8);
    /* quick burst of IR on passed pin */
    delay(1);
    int ir = digitalRead(irReceiverPin);
    /* now read on the reciever pin */
    if (irLedPin == IrLedLeftPin) {
        /* turn the light on for the corresponding LED */
        if (ir == 0) {
            digitalWrite(LedLeftPin, HIGH);
        } else {
            digitalWrite(LedLeftPin, LOW);
        }
    }
    if (irLedPin == IrLedRightPin) {
        if (ir == 0) {
            digitalWrite(LedRightPin, HIGH);
        } else {
            digitalWrite(LedRightPin, LOW);
        }
    }
    delay(1);
    return bool(ir);
}

int GetIrCode() {
    unsigned long irPulse = -1;
    int irCode = 0;

    irPulse = pulseIn(IrDetPin, LOW, 50000);
    /* wait for HIGH then LOW state on IrDetPin w/ 50ms timeout*/

    if (irPulse == 0) {
        return -1;
        /* if timeout: return -1 -- NO-OP */
    }

    for(int i = 0; i <= 11; i++) {
        /* iterate over the 12 bits transmitted */
        irPulse = pulseIn(IrDetPin, LOW, 50000);
        /* Attempt to read an incoming message */

        if ((irPulse > 1000) && (irPulse < 1400)) {
            /* if is in valid range, update the irCode value */
            /* TODO: play with these numbers a bit. We still get random interferance, but we can likely lower tolerance by tuning these */
            bitSet(irCode, i);
            /* Read up on bitwise math if this ^^ is confusing :D */
            /* TODO: Read up on bitwise math ;) */
        }
    }

    Serial.print("Final irCode:");
    Serial.println(irCode);

    if (irCode > 217 && irCode < 225) {
        /* ASCII Compliance */
        irCode -= 89;
    }

    return irCode;
}

void move(int speedLeft, int speedRight) {
    /* default msTime to 20 */
    move(speedLeft, speedRight, 20);
}

void move(int speedLeft, int speedRight, int msTime) {
    Serial.print("move -- speedLeft: ");
    Serial.print(speedLeft);
    Serial.print(" | speedRight: ");
    Serial.print(speedRight);
    Serial.print(" | msTime: ");
    Serial.println(msTime);

    /* logs for debugging */

    servoLeft.writeMicroseconds(1500 + speedLeft);
    servoRight.writeMicroseconds(1500 - speedRight);
    if (msTime == -1) {
        /* send msTime of -1 to make it force stop entirely */
        servoLeft.detach();
        servoRight.detach();
    } else {
        /* gotta wait for the operation to complete */
        delay(msTime);
    }
}

void alert_left() {
    /* helper func to pass left pin to alert */
    int pins[] = { LedLeftPin };
    alert(pins);
}

void alert_right() {
    /* helper func to pass right pin to alert */
    int pins[] = { LedRightPin };
    alert(pins);
}

void alert_all() {
    /* helper func to pass all pins to alert */
    int pins[] = { LedRightPin, LedLeftPin };
    alert(pins);
}

void alert(int pins[]) {
    int pin_length = sizeof(pins)/sizeof(int);

    for (int i = 0; i <= pin_length; i++) {
        int pin = pins[i];
        Serial.print("Alerting pin: ");
        Serial.println(pin);
        move(0, 0);
        digitalWrite(pin, HIGH);
        tone(BeeperPin, 4500, 20);
        delay(50);
        digitalWrite(pin, LOW);
        delay(50);
        /* flash every pin that is passed */
    }
}

int irDistance(int irLedPin, int irReceivePin) {
    int distance = 0;
    for (long f = FREQUENCY; f <= 42000; f += 1000) {
        /* increasing by a factor of 1 Hz, attempt to read whether or not can detect object at varying frequencies */
        distance += int(irDetect(irLedPin, irReceivePin, f));

        /* thoretical maximum = |(FREQUENCY - 42000) / 1000| (rounded) */
    }
    return distance;
}
