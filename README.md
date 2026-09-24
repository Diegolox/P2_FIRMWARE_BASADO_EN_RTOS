# Firmware basado en FreeRTOS

Prácticas con un ESP32-WROOM-32, PlatformIO y el framework Arduino. Cada carpeta es un proyecto independiente.

| Proyecto | Tareas | Código |
| --- | --- | --- |
| `P2_1_BLINK_LED` | LED (200 ms por ciclo) y «Hola mundo» por UART (1 s) | [`tareas.cpp`](P2_1_BLINK_LED/src/app/tareas.cpp) |
| `P2_2_IMU` | Las dos anteriores y lectura del BNO055 (100 ms; salida por UART cada 1 s) | [`tareas.cpp`](P2_2_IMU/src/app/tareas.cpp) |

En ambos proyectos, `setup()` inicializa los periféricos y crea las tareas. FreeRTOS las ejecuta de forma independiente; `vTaskDelay()` bloquea solo la tarea que lo llama.

## P2_1_BLINK_LED

Dos tareas independientes: un LED con 100 ms encendido y 100 ms apagado, y un mensaje por UART cada segundo.

Código de [`P2_1_BLINK_LED/src/app/tareas.cpp`](P2_1_BLINK_LED/src/app/tareas.cpp):

```cpp
#include <Arduino.h>
#include "app/tareas.h"
#include "hal/LED.h"

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
```

## P2_2_IMU

Añade una tarea que lee el BNO055 cada 100 ms y envía una línea con seis medidas cada diez lecturas. Si el sensor no se inicializa, la tarea no se crea.

Código de [`P2_2_IMU/src/app/tareas.cpp`](P2_2_IMU/src/app/tareas.cpp):

```cpp
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
```

## Organización

- `src/app/tareas.cpp`: funciones de tarea y creación con `xTaskCreate()`.
- `src/hal/LED.cpp`: control del LED.
- `src/sensors/BNO055.cpp`: lectura del sensor en la práctica con IMU.
- `include/`: declaraciones y configuración.
