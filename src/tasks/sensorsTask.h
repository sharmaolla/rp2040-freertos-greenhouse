#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include "FreeRTOS.h"
#include "queue.h"
#include <stdint.h>

typedef struct {
    uint16_t co2_ppm;
    float rh;
    float temp;
    //bool fan_running;
    int fan_speed;
} SensorsData_t;

extern QueueHandle_t SensorsDataQ;
extern QueueHandle_t SensorsDataQ_OLED;

void SensorsTask(void *pvParameters);

#endif // SENSOR_TASK_H