#include <Arduino.h>
#include "app/tareas.h"
#include "hal/LED.h"

/*
    xTaskCreate(
        task_hola_mundo,   // Función que ejecutará la tarea
        "HolaMundo",       // Nombre para identificarla
        2048,              // Memoria de pila para esta tarea, en bytes en ESP32
        nullptr,           // Dato opcional que se pasa a la función
        1,                 // Prioridad
        nullptr            // Dónde guardar su identificador, si lo necesitas
    );


*/

void task_hola_mundo(void *parameter) {
    while (true) {
        Serial.println("Hola mundo");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task_blink_led(void *parameter) {
    while (true) {
        encender_led();
        vTaskDelay(pdMS_TO_TICKS(100));
        apagar_led();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void init_task_hola_mundo() {
    xTaskCreate(task_hola_mundo, "HOLA_MUNDO", 4096, nullptr, 1, nullptr);
}

void init_task_blink_led() {
    xTaskCreate(task_blink_led, "BLINK_LED", 2048, nullptr, 1, nullptr);
}