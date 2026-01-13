/*
 * json.c
 *
 *  Created on: Oct 28, 2025
 *      Author: lidiakocon
 */

#include <json_STM.h>
#include "main.h"
#include <stdio.h>
#include <string.h>

int json_create(const char *name, int value, char *buffer, uint16_t buffer_size)
{
    if (name == NULL || buffer == NULL) {
        return -1;
    }

    // Przykład JSON-a: {"name": "temperature", "value": 25}
    int written = snprintf(buffer, buffer_size, "{\"name\": \"%s\", \"value\": %d}", name, value);

    if (written < 0 || written >= buffer_size) {
        // Błąd formatowania lub zbyt mały bufor
        return -1;
    }

    return 0;
}
