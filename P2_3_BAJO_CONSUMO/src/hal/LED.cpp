#include <Arduino.h>
#include "config.h"
#include "hal/LED.h"

void init_led_ESP32() {
    pinMode(LED_ESP32, OUTPUT);
    apagar_led();
}

void encender_led() {
    digitalWrite(LED_ESP32, HIGH);
}

void apagar_led() {
    digitalWrite(LED_ESP32, LOW);
}