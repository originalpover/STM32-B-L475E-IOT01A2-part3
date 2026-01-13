/*
 * Tutaj robimy konfigurację LIS3MDL, czyli podłączamy bibliotekę czujnika do sprzętowego I2C w STM32.
 * Dzięki temu nie trzeba pisać HALa ręcznie. Jest też funkcja do odczytu X/Y/Z i wysyłania wyników przez UART1.
 */

#include <stdio.h>
#include "lis3mdl_conf.h"

LIS3MDL_Object_t lis3mdl;          // globalny obiekt czujnika LIS3MDL – przechowuje konfigurację i stan czujnika
extern UART_HandleTypeDef huart1;  // uchwyt do interfejsu UART1 zdefiniowany w main.c – używany do komunikacji tekstowej

/*
 * Funkcja inicjalizująca czujnik magnetyczny LIS3MDL i konfigurująca jego komunikację z mikrokontrolerem.
 * Tworzy kontekst wejścia/wyjścia (io_ctx) dla sterownika i rejestruje go w strukturze czujnika.
 * Następnie uruchamia czujnik i aktywuje pomiar trzech osi pola magnetycznego (X, Y, Z).
 */
void LIS3MDL_Platform_Init(void)
{
    LIS3MDL_IO_t io_ctx; // struktura opisująca sposób komunikacji czujnika z mikrokontrolerem

    io_ctx.Init    = BSP_I2C2_Init;       // wskaźnik do funkcji inicjalizującej magistralę I2C2
    io_ctx.DeInit  = BSP_I2C2_DeInit;     // wskaźnik do funkcji deinicjalizującej I2C2
    io_ctx.BusType = LIS3MDL_I2C_BUS;     // typ magistrali – tutaj I2C
    io_ctx.Address = LIS3MDL_I2C_ADD_H;   // adres czujnika LIS3MDL (0x3D przy połączeniu SDO = VCC)
    io_ctx.WriteReg= BSP_I2C2_WriteReg;   // funkcja zapisu rejestrów przez I2C
    io_ctx.ReadReg = BSP_I2C2_ReadReg;    // funkcja odczytu rejestrów przez I2C
    io_ctx.GetTick = (LIS3MDL_GetTick_Func)HAL_GetTick; // funkcja zwracająca aktualny licznik systemowy (ms)
    io_ctx.Delay   = HAL_Delay;            // funkcja opóźnienia (blokująca, używana w operacjach inicjalizacyjnych)

    // zarejestruj połączenie z czujnikiem – przypisz powyższe funkcje do struktury sterownika
    LIS3MDL_RegisterBusIO(&lis3mdl, &io_ctx);

    // inicjalizacja czujnika i aktywacja pomiaru pola magnetycznego we wszystkich osiach
    LIS3MDL_Init(&lis3mdl);
    LIS3MDL_MAG_Enable(&lis3mdl);
}

/*
 * Funkcja odczytuje aktualne wartości wektorowe pola magnetycznego (X, Y, Z) z czujnika LIS3MDL.
 * Jeśli pomiar zakończy się powodzeniem, dane są sformatowane i wysyłane przez UART1 w postaci tekstowej.
 * Przykład formatu: "X: -145  Y: 208  Z: 1023".
 */
void LIS3MDL_Read_Magnetic(void)
{
    LIS3MDL_Axes_t mag; // struktura przechowująca dane pomiarowe dla trzech osi pola magnetycznego

    // odczyt danych z czujnika – funkcja zwraca LIS3MDL_OK w przypadku poprawnego pomiaru
    if (LIS3MDL_MAG_GetAxes(&lis3mdl, &mag) == LIS3MDL_OK)
    {
        char msg[64]; // bufor tekstowy dla danych do wysłania
        // formatowanie danych do czytelnej postaci (dane typu int32_t)
        int len = snprintf(msg, sizeof(msg), "X: %ld  Y: %ld  Z: %ld\r\n", mag.x, mag.y, mag.z);
        // wysyłanie danych przez UART1 w trybie blokującym
        HAL_UART_Transmit(&huart1, (uint8_t*)msg, len, HAL_MAX_DELAY);
    }
}
