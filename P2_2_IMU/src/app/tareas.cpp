#include <Arduino.h>
#include "app/tareas.h"
#include "hal/LED.h"
#include "sensors/BNO055.h"

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

void task_print_IMU(void *parameter){
    int i = 0;
    while (true){
        DatosBNO055 datos = leerBNO055();
        i += 1;
        if (i >= 10){
            Serial.printf("%.2f;%.2f;%.2f;%.2f;%.2f;%.2f\n",
                        datos.rumbo, datos.roll, datos.pitch,
                        datos.accX, datos.accY, datos.accZ);
            i = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void init_task_print_IMU() {
    if (!initBNO055()) {
        Serial.println("ERROR: no se ha encontrado el BNO055");
        return;
    }

    Serial.println("BNO055 iniciado");
    xTaskCreate(task_print_IMU, "PRINT_IMU", 4096, nullptr, 1, nullptr);
}


void init_task_hola_mundo() {
    xTaskCreate(task_hola_mundo, "HOLA_MUNDO", 4096, nullptr, 1, nullptr);
}

void init_task_blink_led() {
    xTaskCreate(task_blink_led, "BLINK_LED", 2048, nullptr, 1, nullptr);
}