#pragma once
#include <memory>
#include "ModbusClient.h"
#include "ModbusRegister.h"

class CO2Sensor{
public:
    CO2Sensor(std::shared_ptr<ModbusClient> client);
    int read_ppm();
private:
    ModbusRegister co2_reg;
};