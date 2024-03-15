/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "TMAG5273.hpp"
#include <cstdio>

#define I2C1_SDA p9
#define I2C1_SCL p10
#define I2C2_SDA p28
#define I2C2_SCL p27



void trig1D(float leftMag, float rightMag) {
    //This is some experimentally found function 
    float distRight = sqrtf(((11000.0f / rightMag) - 1) + 4); //sensor 2 dist to the magnet
    float distLeft = sqrtf(((11000.0f / leftMag) - 1) + 4); //sensor 1 dist to the magnet


    float angB = acosf(-(distRight * distRight - distLeft * distLeft - 9.0f) / (2 * 3.0f * distLeft));
    float dx = distLeft * sinf(angB - 3.1415f / 2);
    float centerDist = dx + 1.5f; // positive is left

    if ((int)(angB * 1000) == 0) { //if angle is invalid, use one for distance 
        //if magnet is far enough usually angB calculation fails 
        if (distRight < distLeft) {
            centerDist = -sqrtf(distRight * distRight - 4) - 1.5f;
        } else {
            centerDist = sqrtf(distLeft * distLeft - 4) + 1.5f;
        }
    }
    //Debug statements 
    printf("dist left: %d, right: %d\n", (int)(distLeft * 10), (int)(distRight * 10));
    printf("ang b deg: %d\n", (int)(angB * 180.0f / 3.1415f));

    //the useful data 
    printf("distance in in 0.1: %d\n", (int)(10 * centerDist));
    printf("\n");
}


int main()
{
    TMAG5273 hallSensor1 (I2C1_SDA, I2C1_SCL); 
    TMAG5273 hallSensor2 (I2C2_SDA, I2C2_SCL); 
    wait_us(100000);

    printf("starting calibration... \n");
    int x1, y1, z1;
    int x2, y2, z2;

    float leftMag, rightMag;
    hallSensor1.calibrate();
    hallSensor2.calibrate();
    hallSensor1.oversampleAmount = 9;
    hallSensor2.oversampleAmount = 9;
    printf("calibration done\n");

    while(1) {
        printf("sampling \n");
        hallSensor1.getHallData(&x1, &y1,  &z1);
        hallSensor2.getHallData(&x2, &y2,  &z2);

        hallSensor1.getMagnitude(&leftMag);
        printf("Sensor 1 - Mag: %d\n", (int)leftMag);
        hallSensor2.getMagnitude(&rightMag);
        printf("Sensor 2 - Mag: %d\n", (int)rightMag);

        trig1D(leftMag, rightMag);
        // dist 0 is ~1700
        // dist 1 is ~1050
        // dist 2 is ~407
         
         wait_us(2000000);
    }
    
}
