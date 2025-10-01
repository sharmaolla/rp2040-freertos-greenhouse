#include "co2_valve.h"
#include <stdio.h>

//Constructor

Co2Valve::Co2Valve(uint pin) :valve_pin(pin)
{
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, false);
    printf("CO2 Valve initialized\n");
}

void Co2Valve::OpenValve() {
    gpio_put(valve_pin, true);
}

void Co2Valve::CloseValve() {
    gpio_put(valve_pin, false);
}