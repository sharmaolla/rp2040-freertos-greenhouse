#pragma once
#include "pico/stdlib.h"

class Co2Valve{
public:
    Co2Valve(uint pin);
    void OpenValve();
    void CloseValve();

private:
    uint valve_pin;
};