#pragma once

bool initMutexIMU();

void init_task_hola_mundo();
void init_task_blink_led();

// Inicializa el BNO055 y crea las tareas de lectura e impresión.
void init_task_read_IMU();