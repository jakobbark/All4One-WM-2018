
#define TIMEOUT 1500
#define PIN_OFFSET 2
#define THRESHOLD 4

#include <Wire.h>
#include <SoftPWM.h>
SOFTPWM_DEFINE_CHANNEL(2, DDRD, PORTD, PORTD2);  //Arduino pin 2
SOFTPWM_DEFINE_CHANNEL(3, DDRD, PORTD, PORTD3);  //Arduino pin 3
SOFTPWM_DEFINE_CHANNEL(4, DDRD, PORTD, PORTD4);  //Arduino pin 4
SOFTPWM_DEFINE_CHANNEL(5, DDRD, PORTD, PORTD5);  //Arduino pin 5
SOFTPWM_DEFINE_CHANNEL(6, DDRD, PORTD, PORTD6);  //Arduino pin 6
SOFTPWM_DEFINE_CHANNEL(7, DDRD, PORTD, PORTD7);  //Arduino pin 7
SOFTPWM_DEFINE_CHANNEL(8, DDRB, PORTB, PORTB0);  //Arduino pin 8
SOFTPWM_DEFINE_CHANNEL(9, DDRB, PORTB, PORTB1);  //Arduino pin 9
SOFTPWM_DEFINE_CHANNEL(10, DDRB, PORTB, PORTB2);  //Arduino pin 10
SOFTPWM_DEFINE_CHANNEL(11, DDRB, PORTB, PORTB3);  //Arduino pin 11

SOFTPWM_DEFINE_OBJECT_WITH_PWM_LEVELS(10, 128);

int towBarTargetPosition = 512;

long lastUpdate;

void setup() {
  Serial.begin(9600);

  lastUpdate = millis();

  Wire.begin(0x31);
  Wire.onReceive(receiveEvent);

  Palatis::SoftPWM.begin(60);

  Palatis::SoftPWM.set(7, 128);

  for (size_t i = 0; i < 10; i++) { // MotorDriver outputs
    pinMode(i + PIN_OFFSET, OUTPUT);
  }
}

void loop() {
  //TIMEOUT -> Alles geht aus
  if (millis() > lastUpdate + TIMEOUT) {
    for (size_t i = 0; i < 10; i++) {
      Palatis::SoftPWM.set(i + PIN_OFFSET, 0);
    }
  }

  //TowBar: soll mit Poti vergleichen und bewegen
  int difference = analogRead(A0) - towBarTargetPosition;
      size_t pin = PIN_OFFSET + 8;
  if (difference > THRESHOLD) {
    Palatis::SoftPWM.set(pin, 128 + difference / 4);
    Palatis::SoftPWM.set(pin + 1, 0);
  } else if (difference < -THRESHOLD) {
    Palatis::SoftPWM.set(pin + 1, 128 - difference / 4);
    Palatis::SoftPWM.set(pin, 0);
  } else {
    Palatis::SoftPWM.set(pin, 0);
    Palatis::SoftPWM.set(pin + 1, 0);
  }
}


void receiveEvent(int howMany) {
  lastUpdate = millis();
  if (howMany == 2) {
    int ident = Wire.read();
    int value = Wire.read() - 128;

    if ((ident & 0b00111100) == 32) { // TowBar
      towBarTargetPosition = value;
    }

    if ((ident & 0b00111100) == 48) { // Throttle
      size_t pin = PIN_OFFSET + ((ident & 0b00000011) * 2);
      if (value > 0) {
        Palatis::SoftPWM.set(pin, value);
        Palatis::SoftPWM.set(pin + 1, 0);
      } else if (value < 0) {
        Palatis::SoftPWM.set(pin + 1, -value);
        Palatis::SoftPWM.set(pin, 0);
      } else {
        Palatis::SoftPWM.set(pin, 0);
        Palatis::SoftPWM.set(pin + 1, 0);
      }
    }

  }
}