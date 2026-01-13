/*
 * W tym pliku podpinamy HTS221 do magistrali I2C2 i tworzymy proste funkcje do odczytu temperatury i wilgotności.
 * Dane są wysyłane przez UART1 w formacie tekstowym.
 */

#include <stdio.h>
#include "hts221_conf.h"

HTS221_Object_t hts221;           // globalny obiekt czujnika HTS221 – struktura przechowująca jego konfigurację i stan
extern UART_HandleTypeDef huart1; // UART1 zdefiniowany w main.c – używany do transmisji danych pomiarowych

/*
 * Funkcja inicjalizująca czujnik HTS221 i konfigurująca jego komunikację przez I2C2.
 * Tworzy strukturę kontekstu IO (io_ctx), rejestruje ją w sterowniku HTS221 oraz aktywuje pomiary temperatury i wilgotności.
 */
void HTS221_Platform_Init(void)
{
    HTS221_IO_t io_ctx; // struktura opisująca sposób komunikacji z czujnikiem (funkcje niskiego poziomu)

    io_ctx.Init    = BSP_I2C2_Init;       // wskaźnik do funkcji inicjalizującej magistralę I2C2
    io_ctx.DeInit  = BSP_I2C2_DeInit;     // wskaźnik do funkcji deinicjalizującej I2C2
    io_ctx.BusType = HTS221_I2C_BUS;      // określenie typu magistrali – w tym przypadku I2C
    io_ctx.Address = HTS221_I2C_ADDRESS;  // adres 7-bitowy czujnika HTS221 (0xBE >> 1 = 0x5F)
    io_ctx.WriteReg= BSP_I2C2_WriteReg;   // funkcja do zapisu rejestrów przez I2C
    io_ctx.ReadReg = BSP_I2C2_ReadReg;    // funkcja do odczytu rejestrów przez I2C
    io_ctx.GetTick = (HTS221_GetTick_Func)HAL_GetTick; // funkcja zwracająca aktualny tick systemowy (do opóźnień i timeoutów)

    HTS221_RegisterBusIO(&hts221, &io_ctx); // rejestracja kontekstu IO w strukturze czujnika
    HTS221_Init(&hts221);                   // inicjalizacja czujnika HTS221
    HTS221_HUM_Enable(&hts221);             // włączenie kanału pomiarowego wilgotności
    HTS221_TEMP_Enable(&hts221);            // włączenie kanału pomiarowego temperatury
}

/*
 * Funkcja odczytuje bieżące dane z czujnika HTS221 (temperatura i wilgotność)
 * i wysyła je przez UART1 w formacie tekstowym
 */
void HTS221_Read_Data(void)
{
    float humidity = 0;      // zmienna przechowująca wynik pomiaru wilgotności [%RH]
    float temperature = 0;   // zmienna przechowująca wynik pomiaru temperatury [°C]

    // próba odczytu danych z czujnika – funkcje zwracają HTS221_OK, jeśli odczyt się powiódł
    if (HTS221_HUM_GetHumidity(&hts221, &humidity) == HTS221_OK &&
        HTS221_TEMP_GetTemperature(&hts221, &temperature) == HTS221_OK)
    {
        char msg[64]; // bufor tekstowy dla danych do wysłania przez UART
        // formatowanie danych do postaci czytelnej dla użytkownika
        int len = snprintf(msg, sizeof(msg), "T: %.2f C  H: %.2f %%\r\n", temperature, humidity);
        // transmisja sformatowanego ciągu znaków przez UART1 (blokująco)
        HAL_UART_Transmit(&huart1, (uint8_t*)msg, len, HAL_MAX_DELAY);
    }
}
