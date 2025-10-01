// rename for modbus task

#include "controllersTask.h"
#include "co2_valve//co2_valve.h"
#include "sensorsTask.h"
#include <stdio.h>

#define VALVE_PIN 27
#define MIXING_TIME 3000


void ControllersTask( void *pvParameters){
    int fake_co2_setpoint = 1000;
    int fake_co2_critical = 2000;

    Co2Valve co2Valve(VALVE_PIN);
    SensorsData_t ReceivedData;
    bool fan_working = false;

    while(1){
        if(xQueueReceive(SensorsDataQ, &ReceivedData, portMAX_DELAY)==pdTRUE){
            printf("[Controller] Received data: co2 level: %d ppm\n", ReceivedData.co2_ppm);
            printf("[Controller] Received data: rh level: %.1f \n", ReceivedData.rh);
            if (ReceivedData.co2_ppm < (fake_co2_setpoint - 50)){
                if(fan_working){
                    fan_working = false;
                    printf("[Controller] Fan stopped before injection\n");
                    //stop fan here (later)
                }
                uint32_t current_time = xTaskGetTickCount();
                co2Valve.OpenValve();
                printf("[Controller] Opening valve for 1s at tick %lu\n", current_time);
                vTaskDelay(pdMS_TO_TICKS(1000));
                co2Valve.CloseValve();
                vTaskDelay(pdMS_TO_TICKS(MIXING_TIME));
            }
            else if(ReceivedData.co2_ppm >= fake_co2_critical|| ReceivedData.co2_ppm > (fake_co2_setpoint + 50)){
                co2Valve.CloseValve();
                if(!fan_working){
                    //start fan (do later)
                    fan_working = true;
                    printf("[Controller] Fan started (CO2 high)\n");
                }
                else{
                    printf("[Controller] Fan is working (CO2 still high)\n");
                }

            }
            else {
                if(fan_working){
                    fan_working = false;
                    printf("[Controller] Fan stopped (CO2 within range)\n");
                    //stop fan
                }
                else{
                    printf("[Controller] CO2 within range. No action taken.\n");
                }

            }
        }
    }
}





/*#include "controllersTask.h"
#include "co2_valve//co2_valve.h"
#include "sensorsTask.h"
#include <stdio.h>
#include "oledTask.h"
#include "fan/fan.h"

#define VALVE_PIN 27
#define MIXING_TIME 3000


void ControllersTask( void *pvParameters){
    //auto client = static_cast<std::shared_ptr<ModbusClient>*>(pvParameters);
    //int fake_co2_setpoint = 1000;
    int fake_co2_critical = 2000;

    Co2Valve co2Valve(VALVE_PIN);
    Fan sys_fan(*client);

    SensorsData_t ReceivedData;
    while(1){
        if(xQueueReceive(SensorsDataQ, &ReceivedData, portMAX_DELAY)==pdTRUE){
            printf("[Controller] Received data: co2 level: %d ppm\n", ReceivedData.co2_ppm);  // print in one
            printf("[Controller] Received data: rh level: %.1f \n", ReceivedData.rh);
            printf("[Controller] Received data: T level: %.1f \n", ReceivedData.temp);
            printf("[Controller] Received data: FAN speed: %.d \n", sys_fan.getSpeed());
            //printf("[Controller] Received data: FAN running: %s \n", ReceivedData.fan_running? "Yes":"No");
            if (ReceivedData.co2_ppm < (targetCO2 - 50)){

                sys_fan.setSpeed(0);
                printf("[Controller] Fan stopped before injection\n");
                printf("[Controller] FAN speed: %.d \n", sys_fan.getSpeed());

                uint32_t current_time = xTaskGetTickCount();
                co2Valve.OpenValve();
                printf("[Controller] Opening valve for 1s at tick %lu\n", current_time);
                vTaskDelay(pdMS_TO_TICKS(1000));
                co2Valve.CloseValve();
                vTaskDelay(pdMS_TO_TICKS(MIXING_TIME));
            }
            else if (ReceivedData.co2_ppm >= (fake_co2_critical - 50)){
                sys_fan.setSpeed(100);
                printf("[Controller] Fan is working in full speed (CO2 critically high)\n");
            }
            else if(ReceivedData.co2_ppm > (targetCO2 + 50)){
                co2Valve.CloseValve();

                sys_fan.setSpeed(50);
                printf("[Controller] FAN speed: %.d \n", sys_fan.getSpeed());
                printf("[Controller] Fan is working (CO2 high)\n");

            }


            else{
                sys_fan.setSpeed(0);
                co2Valve.CloseValve();
                printf("[Controller] CO2 within range. FAN is off, VALVE closed\n");
            }
        }
    }
}*/