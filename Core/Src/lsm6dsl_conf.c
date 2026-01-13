/*
   Implementacja konfiguracji czujnika LSM6DSL
   Ustawienia i odczyt danych z akcelerometru i żyroskopu.
   Wykorzystuje I2C2 (na płytce B-L475E-IOT01A2).
*/

#include "lsm6dsl_conf.h"

extern UART_HandleTypeDef huart1;   // uchwyt do interfejsu UART1 z main.c – używany do wysyłania danych pomiarowych

LSM6DSL_Object_t lsm6dsl;           // globalny obiekt czujnika LSM6DSL – struktura przechowująca konfigurację i stan czujnika

/*
 * Funkcja inicjalizująca czujnik LSM6DSL oraz konfigurująca jego połączenie z mikrokontrolerem.
 * Tworzy i wypełnia strukturę kontekstu wejścia/wyjścia (io_ctx), rejestruje ją w sterowniku
 * i aktywuje kanały pomiarowe akcelerometru oraz żyroskopu.
 */
void LSM6DSL_Platform_Init(void)
{
    LSM6DSL_IO_t io_ctx; // struktura opisująca interfejs komunikacyjny czujnika

    io_ctx.BusType  = LSM6DSL_I2C_BUS;     // wybór magistrali – w tym przypadku I2C
    io_ctx.Address  = LSM6DSL_I2C_ADD_L;   // adres czujnika (0x6A przy SDO = GND)
    io_ctx.Init     = BSP_I2C2_Init;       // funkcja inicjalizująca magistralę I2C2
    io_ctx.DeInit   = BSP_I2C2_DeInit;     // funkcja deinicjalizująca I2C2
    io_ctx.ReadReg  = BSP_I2C2_ReadReg;    // funkcja odczytu rejestru czujnika przez I2C
    io_ctx.WriteReg = BSP_I2C2_WriteReg;   // funkcja zapisu rejestru czujnika przez I2C
    io_ctx.GetTick  = (LSM6DSL_GetTick_Func)HAL_GetTick; // funkcja zwracająca aktualny tick systemowy (ms)

    // rejestracja funkcji komunikacyjnych w strukturze czujnika
    if (LSM6DSL_RegisterBusIO(&lsm6dsl, &io_ctx) != LSM6DSL_OK)
        return; // przerwij, jeśli rejestracja się nie powiodła

    // inicjalizacja czujnika – ustawienia domyślne z biblioteki ST
    if (LSM6DSL_Init(&lsm6dsl) != LSM6DSL_OK)
        return; // przerwij, jeśli czujnik nie został poprawnie zainicjalizowany

    // włączenie akcelerometru i żyroskopu
    LSM6DSL_ACC_Enable(&lsm6dsl);   // aktywacja pomiaru przyspieszeń w osiach X/Y/Z
    LSM6DSL_GYRO_Enable(&lsm6dsl);  // aktywacja pomiaru prędkości kątowych w osiach X/Y/Z
}

/*
 * Funkcja odczytuje dane z akcelerometru i żyroskopu czujnika LSM6DSL.
 * W przypadku poprawnego odczytu, wartości dla osi X/Y/Z są formatowane
 * do postaci tekstowej i wysyłane przez UART1.
 */
void LSM6DSL_Read_Data(void)
{
    LSM6DSL_Axes_t acc;   // struktura na dane z akcelerometru (przyspieszenie w mg)
    LSM6DSL_Axes_t gyro;  // struktura na dane z żyroskopu (prędkość kątowa w mdps)
    char msg[100];        // bufor do formatowania tekstu przed wysłaniem

    // odczyt danych z akcelerometru i żyroskopu; obie funkcje muszą zwrócić LSM6DSL_OK
    if (LSM6DSL_ACC_GetAxes(&lsm6dsl, &acc) == LSM6DSL_OK &&
        LSM6DSL_GYRO_GetAxes(&lsm6dsl, &gyro) == LSM6DSL_OK)
    {
        // zapis sformatowanych danych do bufora w formacie czytelnym dla użytkownika
        int len = snprintf(msg, sizeof(msg),
                           "ACC: X=%ld Y=%ld Z=%ld\r\nGYRO: X=%ld Y=%ld Z=%ld\r\n",
                           acc.x, acc.y, acc.z, gyro.x, gyro.y, gyro.z);

        // wysyłanie danych pomiarowych przez UART1 (tryb blokujący, bez przerwań)
        HAL_UART_Transmit(&huart1, (uint8_t*)msg, len, HAL_MAX_DELAY);
    }
}
