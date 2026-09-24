#include <Arduino.h>
#include "config.h"
#include "hal/LED.h"
#include "app/tareas.h"

void setup() {
    Serial.begin(BAUDRATE_SERIAL);
    init_led_ESP32();

    init_task_hola_mundo();
    init_task_blink_led();
}

void loop() {
}