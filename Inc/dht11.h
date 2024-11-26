#include "main.h"
#include <stdint.h>

void DHTInit();

uint8_t DHTResponse();

int DHTReadOneByte();

int DHTRead(uint8_t *humidity, uint8_t *temperature);

void sendToUart(uint8_t *humidity, uint8_t *temperature);