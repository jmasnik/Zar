#include <Servo.h>

Servo servo_a;

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
    servo_a.attach(9);

    digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
    servo_a.writeMicroseconds(1000);
    delay(1000);
    servo_a.writeMicroseconds(1500);
    delay(1000);
    servo_a.writeMicroseconds(2000);
    delay(1000);
    servo_a.writeMicroseconds(1500);
    delay(1000);
}
