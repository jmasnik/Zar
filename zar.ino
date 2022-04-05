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
    uint16_t center;
    uint16_t deadband;
    uint16_t max;
    uint16_t min;
};

inputZar input_otaceni;
inputZar input_vyska;
inputZar input_funkce;

uint32_t tm_ms;
uint32_t tm_ms_prev;
uint32_t tm_ms_print;

int8_t fce_sel;
uint16_t fce_tm;
uint8_t fce_done;

uint8_t reflektor_state;
uint8_t osvetleni_state;

/**
 * Nastaveni
 */
void setup() {

    fce_sel = 0;
    fce_tm = 0;
    fce_done = 0;
    reflektor_state = 0;
    osvetleni_state = 0;

    // vstup otaceni
    input_otaceni.pin = A0;
    input_otaceni.value = 0;
    input_otaceni.center = 1480;
    input_otaceni.deadband = 50;
    input_otaceni.max = 1890;
    input_otaceni.min = 1080;

    // vstup vyska
    input_vyska.pin = A1;
    input_vyska.value = 0;
    input_vyska.center = 1480;
    input_vyska.deadband = 50;
    input_vyska.max = 1903;
    input_vyska.min = 1083;

    // vstup prepinac
    input_funkce.pin = A2;
    input_funkce.value = 0;
    input_funkce.center = 1490;
    input_funkce.deadband = 50;
    input_funkce.max = 1900;
    input_funkce.min = 1120;

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
    servo_otaceni.limit_from = 840;
    servo_otaceni.limit_center = 1570;
    servo_otaceni.limit_to = 2300;
    servo_otaceni.position = 1570;
    
    // servo vyskove
    servo_vyska.servo.attach(10);
    servo_vyska.limit_from = 1165;
    servo_vyska.limit_center = 1340;
    servo_vyska.limit_to = 1720;
    servo_vyska.position = 1340;

    tm_ms_print = 0;
    tm_ms = millis();
    tm_ms_prev = millis();
}

/**
 * Smycka
 */
void loop(){
    int serial_data;
    char str_buff[100];
    int8_t fce_act;

    uint8_t rate_change;

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
        if(serial_data == 'e'){
            servo_vyska.position--;
        }
        if(serial_data == 'd'){
            servo_vyska.position++;
        }
    }

    // nacteni signalu z prijimace
    digitalWrite(LED_BUILTIN, HIGH);
    input_otaceni.value = pulseIn(input_otaceni.pin, HIGH);
    input_vyska.value = pulseIn(input_vyska.pin, HIGH);
    input_funkce.value = pulseIn(input_funkce.pin, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
    
    // cas
    tm_ms_prev = tm_ms;
    tm_ms = millis();

    // ovladani
    if(input_otaceni.value < input_otaceni.center - input_otaceni.deadband){
        // otaceni doprava
        rate_change = 1 + floor((input_otaceni.center - input_otaceni.deadband - input_otaceni.value) / 25.0);
        if(servo_otaceni.position + rate_change < servo_otaceni.limit_to){
            servo_otaceni.position += rate_change;
        } else {
            servo_otaceni.position = servo_otaceni.limit_to;
        }
    }
    if(input_otaceni.value > input_otaceni.center + input_otaceni.deadband){
        // otaceni doleva
        rate_change = 1 + floor((input_otaceni.value - input_otaceni.center + input_otaceni.deadband) / 25.0);
        if(servo_otaceni.position - rate_change > servo_otaceni.limit_from){
            servo_otaceni.position -= rate_change;
        } else {
            servo_otaceni.position = servo_otaceni.limit_from;
        }
    }
    if(input_vyska.value < input_vyska.center - input_vyska.deadband){
        // vyska nahoru
        rate_change = 1 + floor((input_vyska.center - input_vyska.deadband - input_vyska.value) / 20.0);
        if(servo_vyska.position + rate_change < servo_vyska.limit_to){
            servo_vyska.position += rate_change;
        } else {
            servo_vyska.position = servo_vyska.limit_to;
        }
    }
    if(input_vyska.value > input_vyska.center + input_vyska.deadband){
        // vyska dolu
        rate_change = 1 + floor((input_vyska.value - input_vyska.center + input_vyska.deadband) / 20.0);
        if(servo_vyska.position - rate_change > servo_vyska.limit_from){
            servo_vyska.position -= rate_change;
        } else {
            servo_vyska.position = servo_vyska.limit_from;
        }
    }
    if(input_funkce.value > input_funkce.center + input_funkce.deadband){
        fce_act = 1;
        if(input_funkce.value > input_funkce.max - ((input_funkce.max - input_funkce.center - input_funkce.deadband) / 2)) fce_act = 2;
    }
    if(input_funkce.value <= input_funkce.center + input_funkce.deadband && input_funkce.value >= input_funkce.center - input_funkce.deadband){
        fce_act = 0;
    }

    if(fce_act == fce_sel){
        fce_tm += tm_ms - tm_ms_prev;
    } else {
        fce_sel = fce_act;
        fce_tm = 0;
        fce_done = 0;
    }

    // ma se neco stat
    if(fce_sel != 0 && fce_tm > 1000 && fce_done == 0){

        // velky reflektor
        if(fce_sel == 2){
            if(reflektor_state == 0){
                digitalWrite(PIN_SWITCH_D, HIGH);
                reflektor_state = 1;
                fce_done = 1;
            } else {
                digitalWrite(PIN_SWITCH_D, LOW);
                reflektor_state = 0;
                fce_done = 1;
            }   
        }

        // osvetleni
        if(fce_sel == 1){
            if(osvetleni_state == 0){
                digitalWrite(PIN_SWITCH_C, HIGH);
                osvetleni_state = 1;
                fce_done = 1;
            } else {
                digitalWrite(PIN_SWITCH_C, LOW);
                osvetleni_state = 0;
                fce_done = 1;
            }   
        }        
    }

    if(tm_ms - tm_ms_print > 500){
        sprintf(str_buff, "T %lu | O %lu V %lu F %lu | O %u V %u", tm_ms / 1000, input_otaceni.value, input_vyska.value, input_funkce.value, servo_otaceni.position, servo_vyska.position);
        Serial.println(str_buff);
        tm_ms_print = tm_ms;
    }
}
