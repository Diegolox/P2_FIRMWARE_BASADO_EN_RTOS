#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include "app/tareas.h"
#include "hal/LED.h"
#include "sensors/BNO055.h"

// Última muestra del BNO055, compartida por las tareas de lectura e impresión.
static DatosBNO055 ultimaIMU{};
static SemaphoreHandle_t mutexIMU = nullptr;

/* Crea el mutex antes de iniciar las tareas que acceden a ultimaIMU.
 * Si ya existe, no se crea otro. Devuelve false si falta memoria.
 */
bool initMutexIMU() {
    if (mutexIMU == nullptr) {
        mutexIMU = xSemaphoreCreateMutex();
    }
    return mutexIMU != nullptr;
}

void task_hola_mundo(void *parameter) {
    TickType_t t_inicio = xTaskGetTickCount();
    const TickType_t periodo = pdMS_TO_TICKS(1000);

    while (true) {
        Serial.println("Hola mundo");
        vTaskDelayUntil(&t_inicio, periodo);
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

/* Lee el BNO055 cada 100 ms y guarda la última muestra disponible.
 * El acceso I2C se hace fuera del mutex. El mutex solo protege la copia
 * de la estructura compartida, para que la otra tarea no lea datos a medias.
 */
void task_read_IMU(void *parameter) {
    TickType_t t_inicio = xTaskGetTickCount();
    const TickType_t periodo = pdMS_TO_TICKS(100);

    while (true) {
        DatosBNO055 datos = leerBNO055();

        xSemaphoreTake(mutexIMU, portMAX_DELAY);
        ultimaIMU = datos;
        xSemaphoreGive(mutexIMU);

        // t_inicio avanza un periodo en cada llamada; no acumula el tiempo de lectura.
        vTaskDelayUntil(&t_inicio, periodo);
    }
}

/* Cada segundo copia la última lectura y la envía por Serial.
 * Se libera el mutex antes de imprimir para no bloquear la tarea de lectura
 * durante la transmisión. No consume las muestras intermedias de 100 ms.
 */
void task_print_IMU(void *parameter) {
    TickType_t t_inicio = xTaskGetTickCount();
    const TickType_t periodo = pdMS_TO_TICKS(1000);

    while (true) {
        vTaskDelayUntil(&t_inicio, periodo);

        DatosBNO055 datos;

        xSemaphoreTake(mutexIMU, portMAX_DELAY);
        datos = ultimaIMU;
        xSemaphoreGive(mutexIMU);

        Serial.printf("%.2f;%.2f;%.2f;%.2f;%.2f;%.2f\n",
                      datos.rumbo, datos.roll, datos.pitch,
                      datos.accX, datos.accY, datos.accZ);
    }
}

/* Inicializa el sensor y el mutex y arranca ambas tareas de la IMU.
 * Llamar una sola vez desde setup(), después de Serial.begin().
 */
void init_task_read_IMU() {
    if (!initBNO055()) { // intenta conectarse al imu
        Serial.println("ERROR: no se ha encontrado el BNO055");
        return;
    }

    if (!initMutexIMU()) { // intenta crear el mutex
        Serial.println("ERROR: no se ha podido crear el mutex de la IMU");
        return;
    }

    TaskHandle_t tareaLectura = nullptr;

    // Crea tarea que lee periodicamente el IMU
    if (xTaskCreate(task_read_IMU, "READ_IMU", 4096,
                    nullptr, 2, &tareaLectura) != pdPASS) {
        Serial.println("ERROR: no se ha podido crear la tarea de lectura IMU");
        return;
    }

    // Crea tarea que envia por pantalla periodicamente el IMU
    if (xTaskCreate(task_print_IMU, "PRINT_IMU", 4096,
                    nullptr, 1, nullptr) != pdPASS) {
        vTaskDelete(tareaLectura);
        Serial.println("ERROR: no se ha podido crear la tarea de impresión IMU");
        return;
    }

    Serial.println("BNO055 iniciado");
}

void init_task_hola_mundo() {
    xTaskCreate(task_hola_mundo, "HOLA_MUNDO", 4096, nullptr, 1, nullptr);
}

void init_task_blink_led() {
    xTaskCreate(task_blink_led, "BLINK_LED", 2048, nullptr, 1, nullptr);
}