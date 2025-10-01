#include "sensorsTask.h"
#include "co2_sensor/co2.h"  // ?????????
#include "rh_t_sensor/rh_t_sensor.h"
#include "fan/fan.h"
#include <stdio.h>
#include <math.h>

QueueHandle_t SensorsDataQ = nullptr;
QueueHandle_t SensorsDataQ_OLED = nullptr;

void SensorsTask( void *pvParameters){
    auto client = static_cast<std::shared_ptr<ModbusClient>*>(pvParameters);
    SensorsDataQ = xQueueCreate(1, sizeof(SensorsData_t));
    SensorsDataQ_OLED = xQueueCreate(1, sizeof(SensorsData_t)); // --------------------size?

    if(SensorsDataQ == NULL){
        printf("[Sensor Task]: Failed to create Sensors data queue."); //??????????
    }

    CO2Sensor co2sensor(*client);
    RHTSensor rht_sensor(*client);
    Fan fan(*client);
    SensorsData_t SensorsData;

    while(1){
        int ppm = co2sensor.read_ppm();
        auto rh_value = rht_sensor.read_rh() / 10.0;   // without var
        auto temp_value = rht_sensor.read_t() / 10.0;
        //bool fanRun = fan.isRunning();

        auto fan_speed =  fan.getSpeed() / 10.0;

        printf("[Sensor task] Sent data: co2 read %d\n", ppm);
        printf("[Sensor task] Sent data: RH read %5.1f\n", rh_value);
        printf("[Sensor task] Sent data: T read %5.1f\n", temp_value);
        printf("[Sensor task] data: Fan speed  %5.1f\n", fan_speed);
        fan.setSpeed(50);
        printf("[Sensor task] New Fan speed  %d\n", fan.getSpeed());

        //printf("[Sensor task] Sent data: Fan running: %s ", fanRun? "Yes" : "No");

        if (ppm < 0) {
            printf("[Sensor task] CO2 reading invalid\n");
        }
        if (isnan(rh_value)) {
            printf("[Sensor task] RH reading invalid\n");
        }
        if (isnan(temp_value)) {
            printf("[Sensor task] TEMP reading invalid\n");
        }

        SensorsData.co2_ppm = ppm;
        SensorsData.rh = rh_value;
        SensorsData.temp = temp_value;
        //SensorsData.fan_speed = fan_speed;
        //SensorsData.fan_running = fanRun;
        xQueueOverwrite(SensorsDataQ, &SensorsData);
        xQueueOverwrite(SensorsDataQ_OLED, &SensorsData);
        //xQueueSend(SensorsDataQ, &SensorsData, 0);


        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}