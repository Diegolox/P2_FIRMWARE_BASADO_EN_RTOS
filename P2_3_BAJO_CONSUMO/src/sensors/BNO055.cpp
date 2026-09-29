#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BNO055.h>
#include "config.h"
#include "sensors/BNO055.h"

static Adafruit_BNO055 bno(55, 0x29, &Wire);

bool initBNO055() {
    Wire.begin(I2C_SDA, I2C_SCL);
    return bno.begin();
}

DatosBNO055 leerBNO055() {
    DatosBNO055 datos;

    imu::Vector<3> orientacion =
        bno.getVector(Adafruit_BNO055::VECTOR_EULER);

    imu::Vector<3> aceleracion =
        bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

    datos.rumbo = orientacion.x();
    datos.roll  = orientacion.y();
    datos.pitch = orientacion.z();

    datos.accX = aceleracion.x();
    datos.accY = aceleracion.y();
    datos.accZ = aceleracion.z();

    return datos;
}