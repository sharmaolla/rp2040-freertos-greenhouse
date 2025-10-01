#include "rh_t_sensor.h"
#include <stdio.h>

//Constructor

RHTSensor::RHTSensor(std::shared_ptr<ModbusClient> client)
    : rh_reg(client, 241, 256), t_reg(client, 241, 257)
{
    printf("RHT Sensor initialized\n");
}

float RHTSensor::read_rh() {
    uint16_t rh_value = rh_reg.read();
    if(rh_value < 0){   // lowest limit  ????????????????
        printf("RH reading error\n");
        return -1; // return NAN
    }
    return static_cast<float>(rh_value);
}

float RHTSensor::read_t() {
    uint16_t rh_value = t_reg.read();
    return static_cast<float>(rh_value);
}