#include <Servo.h>

Servo servo_a;
Servo servo_b;

#define PIN_SWITCH_A 5
#define PIN_SWITCH_B 6
#define PIN_SWITCH_C 3
#define PIN_SWITCH_D 11

struct servoZar {
    Servo servo;
    uint16_t position;
    uint16_t limit_from;
    uint16_t limit_center;
    uint16_t limit_to;
};

servoZar servo_otaceni;
servoZar servo_vyska;

struct inputZar {
    uint8_t pin;
    unsigned long value;
};

inputZar input_otaceni;
inputZar input_vyska;
inputZar input_funkce;

uint32_t tm_ms_print;

/**
 * Nastaveni
 */
void setup() {
    // vstupy
    input_otaceni.pin = A0;
    input_vyska.pin = A1;
    input_funkce.pin = A2;

    pinMode(input_otaceni.pin, INPUT);
    pinMode(input_vyska.pin, INPUT);
    pinMode(input_funkce.pin, INPUT);

    // vystupy
	pinMode(PIN_SWITCH_A, OUTPUT);
    pinMode(PIN_SWITCH_B, OUTPUT);
    pinMode(PIN_SWITCH_C, OUTPUT);
    pinMode(PIN_SWITCH_D, OUTPUT);
    digitalWrite(PIN_SWITCH_A, LOW);
    digitalWrite(PIN_SWITCH_B, LOW);
    digitalWrite(PIN_SWITCH_C, LOW);
    digitalWrite(PIN_SWITCH_D, LOW);

    // interni LED
	pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    // seriak
    Serial.begin(115200);

    // servo otaceni
    servo_otaceni.servo.attach(9);
    servo_otaceni.limit_from = 1000;
    servo_otaceni.limit_center = 1500;
    servo_otaceni.limit_to = 2000;
    servo_otaceni.position = 1500;
    
    // servo vyskove
    servo_vyska.servo.attach(10);
    servo_vyska.limit_from = 1000;
    servo_vyska.limit_center = 1500;
    servo_vyska.limit_to = 2000;
    servo_vyska.position = 1500;

    tm_ms_print = 0;
}

/**
 * Smycka
 */
void loop(){
    int serial_data;
    char str_buff[50];
    uint32_t tm_ms;

    // nastaveni pozice serv
    servo_otaceni.servo.writeMicroseconds(servo_otaceni.position);
    servo_vyska.servo.writeMicroseconds(servo_vyska.position);

    // ladeni pres seriak
    if(Serial.available() > 0) {
        serial_data = Serial.read();
        if(serial_data == 'q'){
            servo_otaceni.position--;
        }
        if(serial_data == 'w'){
            servo_otaceni.position++;
        }
        sprintf(str_buff, "O: %u  V: %u", servo_otaceni.position, servo_vyska.position);
        Serial.println(str_buff);
    }

    digitalWrite(LED_BUILTIN, HIGH);
    input_otaceni.value = pulseIn(input_otaceni.pin, HIGH);
    input_vyska.value = pulseIn(input_vyska.pin, HIGH);
    input_funkce.value = pulseIn(input_funkce.pin, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
    
    tm_ms = millis();
    if(tm_ms - tm_ms_print > 1000){
        sprintf(str_buff, "%lu / %lu %lu %lu", tm_ms / 1000, input_otaceni.value, input_vyska.value, input_funkce.value);
        Serial.println(str_buff);
        tm_ms_print = tm_ms;
    }
}
