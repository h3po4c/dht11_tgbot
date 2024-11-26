#include "dht11.h"
#include "tim.h"
#include "usart.h"
#include <stdio.h>

uint8_t status;

void DHTInit() 
{
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_1);
    LL_mDelay(18);
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1);
    uDelay(20);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_INPUT);
}

uint8_t DHTResponse() 
{
    uint8_t response = 0;
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_INPUT);
    uDelay(40);
    if(!LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_1)) {
        uDelay(80);
        if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_1)) response = 0;
        else response = 10;
    } else {
        response = 12;
    }
    while(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_1));
    return response;
}

int DHTReadOneByte() {
    uint8_t data = 0;
    for (int i = 0; i < 8; i++) {
        while (!LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_1));
        uDelay(40);

        if (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_1)) {
            data |= (1 << (7 - i));
            while (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_1));
        }
    }
    return data;
}



int DHTRead(uint8_t *humidity, uint8_t *temperature) 
{
    DHTInit();
    status = DHTResponse();
    if (status != 0) {
        LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT); // Установка пина в выход
        LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1); 
        return -1;
    }
    
    uint8_t rh_byte1 = DHTReadOneByte();
    uint8_t rh_byte2 = DHTReadOneByte();
    uint8_t temp_byte1 = DHTReadOneByte();
    uint8_t temp_byte2 = DHTReadOneByte();
    uint8_t checksum = DHTReadOneByte(); 
    uint8_t sum = rh_byte1 + rh_byte2 + temp_byte1 + temp_byte2;

    if (sum != checksum) {
        *humidity = 0;
        *temperature = 0;
        LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT); // Установка пина в выход
        LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1);
        return -1;
    }

    *humidity = rh_byte1;
    *temperature = temp_byte1;
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT); // Установка пина в выход
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1); // Возвращаем высокий уровень
    return 0;
}

void moveCursorUp(uint8_t lines) {
    char command[10];

    // Формируем команду для перемещения вверх на `lines` строк
    sprintf(command, "\033[%dA", lines);

    // Отправляем команду через UART
    HAL_UART_Transmit(&huart1, (uint8_t*)command, strlen(command), HAL_MAX_DELAY);
}

void sendToUart(uint8_t *humidity, uint8_t *temperature)
{
    char buffer[50];

    // Формирование строки для влажности
    sprintf(buffer, "Humidity: %d\n\r", *humidity);
    HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

    // Формирование строки для температуры
    sprintf(buffer, "Temperature: %d\r", *temperature);
    HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
    moveCursorUp(1);
}