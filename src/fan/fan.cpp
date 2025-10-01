#include "fan.h"
#include <stdio.h>

//Constructor

Fan::Fan(std::shared_ptr<ModbusClient> client)
        : AO1_reg(client, 1, 0, true),
        AI1_reg(client, 1, 0, false)
        //AI1_digital_reg(client, 1, 0, false, true)

{
    printf("FAN initialized\n");
}

void Fan::setSpeed(uint16_t percent) {
    AO1_reg.write(percent*10);

}
 uint16_t Fan::getSpeed() {
    uint16_t currentSpeed = AI1_reg.read();
    return currentSpeed;
}