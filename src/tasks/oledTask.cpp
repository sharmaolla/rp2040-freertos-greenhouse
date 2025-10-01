#include "display/ssd1306os.h"
#include "sensorsTask.h"
#include "oledTask.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "queue.h"

#define ENCODER_A  10
#define ENCODER_B  11
#define BUTTON_PIN 12

static UiScreen state = UiScreen::Welcome;
static int menu_index = 0;
QueueHandle_t UiDataQ = nullptr;
int targetCO2 = 500;

void init_encoder(int pin, int  pull_up){

    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    if (pull_up == 1){
        gpio_pull_up(pin);
    }


}
void Encoder_ISR(uint gpio, uint32_t events){

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    static absolute_time_t last_time = nil_time;
    int event;
    bool send = false;

    if(gpio == BUTTON_PIN && (events & GPIO_IRQ_EDGE_FALL) && absolute_time_diff_us(last_time, get_absolute_time())>=300000){
        event = 0;
        send = true;
        last_time = get_absolute_time();
    }
    else if(gpio == ENCODER_A && (events & GPIO_IRQ_EDGE_RISE) && gpio_get(ENCODER_B) == 0){
        event = 1;
        send = true;
    }
    else if(gpio == ENCODER_A && (events & GPIO_IRQ_EDGE_RISE) && gpio_get(ENCODER_B) == 1){
        event = -1;
        send = true;
    }
    if(send){
        xQueueSendFromISR(UiDataQ, &event, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
TickType_t last_activity_time;
int local_co2_target = 500;
void ChangeStateTask(void *pvParameters){
    int received;


   // UiDataQ = xQueueCreate(10,sizeof (int));

    while(1){
        if(xQueueReceive(UiDataQ,& received, portMAX_DELAY)){
            last_activity_time = xTaskGetTickCount();
            printf("received = %d\n\n", received);
            if((received == 0) && (state == UiScreen::Welcome)){
                state = UiScreen::Idle;

            }
            else if((received == 0) && (state == UiScreen::Idle)){
                state = UiScreen::Menu;
            }
            else if((received == 0) && (state == UiScreen::Menu)){
                if(menu_index == 0){
                    state = UiScreen::Set_CO2;
                }
                else if(menu_index == 1){
                    state = UiScreen::Idle;
                }

            }
            else if((received == 0) && (state == UiScreen::Set_CO2)){
                targetCO2 = local_co2_target;
                printf("\n\n New CO2 target set : %d", targetCO2);
                state = UiScreen::Idle;
            }

            if((received == 1) && (state == UiScreen::Menu)){
                menu_index = (menu_index +1) % 2;
            }
            else if((received == -1) && (state == UiScreen::Menu)){
                menu_index = (menu_index - 1 +2 ) % 2;
            }
            if(state == UiScreen::Set_CO2){
                if(received == 1 && local_co2_target < 1500){
                    local_co2_target += 10;
                }
                else if(received == -1 && local_co2_target > 400){
                    local_co2_target -= 10;
                }
            }
        }
    }

}

void OledTask(void *pvParameters)
{

    auto i2cbus{std::make_shared<PicoI2C>(1, 400000)};
    ssd1306os display(i2cbus);
    SensorsData_t ReceivedData;
    char oled_buffer[32];
    //int received;
    state = UiScreen::Welcome;


    init_encoder(ENCODER_A, 0);
    init_encoder(ENCODER_B, 0);
    init_encoder(BUTTON_PIN, 1);
    static TickType_t last_updateOLED = 0;
    static TickType_t state_entry_time = 0;


    while(1){
        if (state == UiScreen::Menu || state == UiScreen::Set_CO2) {

            if (xTaskGetTickCount() - last_activity_time > pdMS_TO_TICKS(10000)) {
                state = UiScreen::Idle;
            }
        }



        switch(state){
            case UiScreen::Welcome:
                display.fill(0);
                display.text("=WELCOME=", 30, 0);
                display.show();
                    if (state_entry_time == 0) {
                        state_entry_time = xTaskGetTickCount();
                    }

                    if (xTaskGetTickCount() - state_entry_time > pdMS_TO_TICKS(5000)) {
                        state = UiScreen::Idle;
                        state_entry_time = 0;
                    }
                //vTaskDelay(pdMS_TO_TICKS(10000));
                break;
            case UiScreen::Idle:

                display.fill(0);
                    display.text("=IDLE=", 30, 0);
                if(xTaskGetTickCount() - last_updateOLED > pdMS_TO_TICKS(1000)){
                    if(xQueueReceive(SensorsDataQ_OLED, &ReceivedData, 0)){

                        //display.fill(0);
                        snprintf(oled_buffer, sizeof(oled_buffer), "CO2: %d ppm", ReceivedData.co2_ppm);
                        display.text(oled_buffer, 0, 16);

                        snprintf(oled_buffer, sizeof(oled_buffer), "RH : %.1f ppm", ReceivedData.rh);
                        display.text(oled_buffer, 0, 36);

                        snprintf(oled_buffer, sizeof(oled_buffer), "T  : %.1f C", ReceivedData.temp);
                        display.text(oled_buffer, 0, 46);

                        display.text("PRS BTN for MENU", 0, 56);


                        display.show();

                    }
                    last_updateOLED = xTaskGetTickCount();
                }
                break;


            case UiScreen::Menu:
                display.fill(0);
                display.text("Menu", 0, 0);
                display.text("Set CO2", 16, 16);
                display.text("IDLE mode", 16, 32);
                if(menu_index == 0){
                    display.text("->", 0, 16);
                }
                if(menu_index == 1){
                    display.text("->", 0, 32);
                }
                display.show();

                //vTaskDelay(pdMS_TO_TICKS(10000));
                break;
            case UiScreen::Set_CO2:
                display.fill(0);
                char buf[32];
                snprintf(buf, sizeof(buf), "SET CO2: %d", local_co2_target);
                display.text(buf, 0, 0);
                display.text("Press to save", 0, 16);
                display.show();
                //vTaskDelay(pdMS_TO_TICKS(10000));
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));

    }

}

/*if(xQueueReceive(SensorsDataQ_OLED, &ReceivedData, portMAX_DELAY)) {
                    display.fill(0);
                    snprintf(oled_buffer, sizeof(oled_buffer), "CO2: %d ppm", ReceivedData.co2_ppm);
                    display.text(oled_buffer, 0, 0);

                    snprintf(oled_buffer, sizeof(oled_buffer), "RH : %.1f ppm", ReceivedData.rh);
                    display.text(oled_buffer, 0, 16);

                    snprintf(oled_buffer, sizeof(oled_buffer), "T  : %.1f C", ReceivedData.temp);
                    display.text(oled_buffer, 0, 32);

                    display.text("PRESS BUTTON TO ENTER MENU", 0, 48);


                    display.show();
                    vTaskDelay(pdMS_TO_TICKS(5000));
                }*/