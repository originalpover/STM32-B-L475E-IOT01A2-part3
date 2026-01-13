#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <stdint.h>

typedef struct {
    int32_t acc_x;
    int32_t acc_y;
    int32_t acc_z;
    int32_t gyro_x;
    int32_t gyro_y;
    int32_t gyro_z;
    float temperature;
    float humidity;
    int32_t mag_x;
    int32_t mag_y;
    int32_t mag_z;
} SensorData_t;

int Sensor_GetData(SensorData_t *data);
int j_johnson(char *json_buffer, uint16_t buffer_size);

#endif
