#include "stm32f1xx_hal.h"

#define DELAY_MS 5  // Задержка для динамической индикации

// Массив сегментов для цифр 0-9 (A0-A6: A,B,C,D,E,F,G)
// Сегменты включаются логическим 0 (если индикатор с общим анодом)
const uint8_t digit_patterns[10] = {
    0b1000000, // 0 (сегменты A,B,C,D,E,F)
    0b1111001, // 1 (сегменты B,C)
    0b0100100, // 2 (сегменты A,B,G,E,D)
    0b0110000, // 3 (сегменты A,B,G,C,D)
    0b0011001, // 4 (сегменты F,G,B,C)
    0b0010010, // 5 (сегменты A,F,G,C,D)
    0b0000010, // 6 (сегменты A,F,G,C,D,E)
    0b1111000, // 7 (сегменты A,B,C)
    0b0000000, // 8 (все сегменты)
    0b0010000  // 9 (сегменты A,B,C,D,F,G)
};

// Время 14:22 (для примера)
uint8_t time_digits[4] = {1, 4, 2, 2};  // 1 4 : 2 2

void GPIO_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();  // Для разрядов (цифр)

    // Настройка сегментов (A0-A6)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                          GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Настройка разрядов (например, PB0-PB3)
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

// Отображение цифры на указанном разряде
void DisplayDigit(uint8_t digit, uint8_t position) {
    // Выключаем все разряды
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_SET);

    // Устанавливаем сегменты для текущей цифры
    uint8_t pattern = digit_patterns[digit];
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, (pattern & 0b0000001) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, (pattern & 0b0000010) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, (pattern & 0b0000100) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, (pattern & 0b0001000) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, (pattern & 0b0010000) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, (pattern & 0b0100000) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, (pattern & 0b1000000) ? GPIO_PIN_RESET : GPIO_PIN_SET);

    // Включаем текущий разряд
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 << position, GPIO_PIN_RESET);
}

int main(void) {
    HAL_Init();
    GPIO_Init();

    while (1) {
        // Динамическая индикация (поочередное отображение цифр)
        for (uint8_t i = 0; i < 4; i++) {
            DisplayDigit(time_digits[i], i);
            HAL_Delay(DELAY_MS);
        }
    }
}

