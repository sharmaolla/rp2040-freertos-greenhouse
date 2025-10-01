#ifndef OLED_TASK_H
#define OLED_TASK_H

#include "FreeRTOS.h"
#include "queue.h"
#include <stdint.h>
#include "ssd1306os.h"

#pragma once
extern int targetCO2;

enum class UiScreen{
    Welcome,
    Idle,
    Menu,
    Set_CO2
};

extern QueueHandle_t UiDataQ;

void Encoder_ISR(uint gpio, uint32_t events);
void init_encoder(int pin, int  pull_up);

void OledTask(void *pvParameters);
void ChangeStateTask(void *pvParameters);

#endif