#pragma once
#include <memory>
#include "ModbusClient.h"
#include "ModbusRegister.h"

class RHTSensor{
public:
    RHTSensor(std::shared_ptr<ModbusClient> client);
    float read_rh();
    float read_t();
private:
    ModbusRegister rh_reg;
    ModbusRegister t_reg;
};