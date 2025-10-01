#pragma once
#include <memory>
#include "ModbusClient.h"
#include "ModbusRegister.h"

class Fan{
public:
    Fan(std::shared_ptr<ModbusClient> client);
    void setSpeed(uint16_t percent);
    uint16_t getSpeed();
    //bool isRunning();


private:
    ModbusRegister AO1_reg;
    ModbusRegister AI1_reg;
    //ModbusRegister AI1_digital_reg;

    uint16_t speed = 0;
};