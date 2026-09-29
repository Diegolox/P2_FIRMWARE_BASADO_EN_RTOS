#ifndef BNO055_H
#define BNO055_H

struct DatosBNO055 {
    float rumbo;
    float roll;
    float pitch;
    float accX;
    float accY;
    float accZ;
};

bool initBNO055();
DatosBNO055 leerBNO055();

#endif