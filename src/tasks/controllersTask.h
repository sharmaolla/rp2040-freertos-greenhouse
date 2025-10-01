#ifndef CONTROLLER_TASK_H
#define CONTROLLER_TASK_H

#include "FreeRTOS.h"
#include "queue.h"
#include <stdint.h>


void ControllersTask(void *pvParameters);

#endif