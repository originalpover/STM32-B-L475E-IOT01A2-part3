#include <stdio.h>
#include "BLE_conf.h"
#include "string.h"
#include "main.h"
#include "sample_service.h"
void BLE_SendMessage(const char* msg)
{
    if (msg == NULL) return;
    uint8_t length = strlen(msg);
    // BLE ma limit 20 bajtów na jeden pakiet w standardowym MTU
    // więc wysyłamy w segmentach po 20 bajtów:
    const uint8_t BLE_MAX_PACKET = 20;

    uint8_t offset = 0;
    while (offset < length)
    {
        uint8_t chunk_len = (length - offset > BLE_MAX_PACKET) ? BLE_MAX_PACKET : (length - offset);
        sendData((uint8_t*)&msg[offset], chunk_len);
        offset += chunk_len;
        HAL_Delay(10);
    }
}
