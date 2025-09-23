#include "co2.h"
#include <stdio.h>

//Constructor

CO2Sensor::CO2Sensor(std::shared_ptr<ModbusClient> client)
    : co2_reg(client, 241, 257, false)
{
    printf("CO2 Sensor initialized\n");
}

int CO2Sensor::read_ppm() {
    uint16_t ppm_value = co2_reg.read();
    if(ppm_value < 0 ){
        printf("CO2 reading error\n");
        return -1;
    }
    return static_cast<int>(ppm_value);
}