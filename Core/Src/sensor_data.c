#include "sensor_data.h"
#include "lsm6dsl_conf.h"
#include "hts221_conf.h"
#include "lis3mdl_conf.h"
#include "json_STM.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

extern LSM6DSL_Object_t lsm6dsl;
extern HTS221_Object_t hts221;
extern LIS3MDL_Object_t lis3mdl;

int Sensor_GetData(SensorData_t *data)
{
    if (data == NULL)
        return -1;

    // --- LSM6DSL ---
    LSM6DSL_Axes_t acc, gyro;
    if (LSM6DSL_ACC_GetAxes(&lsm6dsl, &acc) != LSM6DSL_OK ||
        LSM6DSL_GYRO_GetAxes(&lsm6dsl, &gyro) != LSM6DSL_OK)
        return -1;

    data->acc_x = acc.x;
    data->acc_y = acc.y;
    data->acc_z = acc.z;
    data->gyro_x = gyro.x;
    data->gyro_y = gyro.y;
    data->gyro_z = gyro.z;

    // --- HTS221 ---
    HTS221_HUM_GetHumidity(&hts221, &data->humidity);
    HTS221_TEMP_GetTemperature(&hts221, &data->temperature);

    // --- LIS3MDL ---
    LIS3MDL_Axes_t mag;
    if (LIS3MDL_MAG_GetAxes(&lis3mdl, &mag) == LIS3MDL_OK)
    {
        data->mag_x = mag.x;
        data->mag_y = mag.y;
        data->mag_z = mag.z;
    }

    return 0;
}

int j_johnson(char *json_buffer, uint16_t buffer_size)
{
    SensorData_t data;
    if (Sensor_GetData(&data) != 0)
        return -1;

    char temp[128];
    int offset = 0;
    memset(json_buffer, 0, buffer_size);




    // --- Sekcja LSM6DSL ---
    offset += snprintf(json_buffer + offset, buffer_size - offset, " { \r\n    \"LSM6DSL\": \r\n");

    json_create("acc_x", data.acc_x, temp, sizeof(temp));
    offset += snprintf(json_buffer + offset, buffer_size - offset, "      %s,\r\n", temp);
    json_create("acc_y", data.acc_y, temp, sizeof(temp));
    offset += snprintf(json_buffer + offset, buffer_size - offset, "      %s,\r\n", temp);
    json_create("acc_z", data.acc_z, temp, sizeof(temp));
    offset += snprintf(json_buffer + offset, buffer_size - offset, "      %s,\r\n", temp);
    json_create("gyro_x", data.gyro_x, temp, sizeof(temp));
    offset += snprintf(json_buffer + offset, buffer_size - offset, "      %s,\r\n", temp);
    json_create("gyro_y", data.gyro_y, temp, sizeof(temp));
    offset += snprintf(json_buffer + offset, buffer_size - offset, "      %s,\r\n", temp);
    json_create("gyro_z", data.gyro_z, temp, sizeof(temp));
    offset += snprintf(json_buffer + offset, buffer_size - offset, "      %s\r\n", temp);

    // --- Sekcja HTS221 ---
    offset += snprintf(json_buffer + offset, buffer_size - offset, "    \"HTS221\": \r\n");

    snprintf(temp, sizeof(temp), "{\"name\": \"temperature\", \"value\": %.f}", data.temperature);
    offset += snprintf(json_buffer + offset, buffer_size - offset, "      %s,\r\n", temp);

    snprintf(temp, sizeof(temp), "{\"name\": \"humidity\", \"value\": %.f}", data.humidity);
    offset += snprintf(json_buffer + offset, buffer_size - offset, "      %s\r\n", temp);


    // --- Sekcja LIS3MDL ---
    offset += snprintf(json_buffer + offset, buffer_size - offset, "    \"LIS3MDL\": \r\n");

    json_create("mag_x", data.mag_x, temp, sizeof(temp));
    offset += snprintf(json_buffer + offset, buffer_size - offset, "      %s,\r\n", temp);
    json_create("mag_y", data.mag_y, temp, sizeof(temp));
    offset += snprintf(json_buffer + offset, buffer_size - offset, "      %s,\r\n", temp);
    json_create("mag_z", data.mag_z, temp, sizeof(temp));
    offset += snprintf(json_buffer + offset, buffer_size - offset, "      %s\r\n", temp);

    offset += snprintf(json_buffer + offset, buffer_size - offset, "    \r\n");

    // --- Zamykanie JSON-a ---
    snprintf(json_buffer + offset, buffer_size - offset, "  }\r\n");

    return 0;
}
