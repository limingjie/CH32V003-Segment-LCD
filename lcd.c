/*
 * CH32V003 Segment LCD
 *
 * Oct 2025 by Li Mingjie
 *  - Email:  limingjie@outlook.com
 *  - GitHub: https://github.com/limingjie/
 */

#include "ch32fun.h"

// TN Positive 3-Digit Segment LCD
//
// LCD PINOUT and Segments
//
//   COM1 2 3 4 SEG6
//   10 | | | | | 6         -A-
//     +---------+        F|   |B
//     | D1 D2 D3|          -G-
//     +---------+        E|   |C
//    1 | | | | | 5         -D-
//   SEG1 2 3 4 5
//
// Segment Matrix and CH32V003 Pin Mapping
//
//            PC7  PC6  PC5  PC4  PC3  PC2
//           SEG6 SEG5 SEG4 SEG3 SEG2 SEG1
// PD4 COM4   1F   1A   2F   2A   3F   3A
// PD5 COM3   1G   1B   2G   2B   3G   3B
// PD6 COM2   1E   1C   2E   2C   3E   3C
// PD0 COM1   1D   __   2D   __   3D   __

#define PIN_COM4 PD4
#define PIN_COM3 PD5
#define PIN_COM2 PD6
#define PIN_COM1 PD0
#define PIN_SEG6 PC7
#define PIN_SEG5 PC6
#define PIN_SEG4 PC5
#define PIN_SEG3 PC4
#define PIN_SEG2 PC3
#define PIN_SEG1 PC2

// Segments bit order: FAGBECD
const uint8_t digit_segments[10] = {
    0b1101111,  // 0: ABCDEF_ = 1011011 -> 1101111
    0b0001010,  // 1: _BC____ = 0110000 -> 0001010
    0b0111101,  // 2: AB_DE_G = 1101101 -> 0111101
    0b0111011,  // 3: ABCD__G = 1111001 -> 0111011
    0b1011010,  // 4: _BC__FG = 0110011 -> 1011010
    0b1110011,  // 5: A_CD_FG = 1011011 -> 1110011
    0b1110111,  // 6: A_CDEFG = 1011111 -> 1110111
    0b0101010,  // 7: ABC____ = 1110000 -> 0101010
    0b1111111,  // 8: ABCDEFG = 1111111 -> 1111111
    0b1111011   // 9: ABCD_FG = 1111011 -> 1111011
};

uint8_t com_pins[4]  = {PIN_COM4, PIN_COM3, PIN_COM2, PIN_COM1};
uint8_t com_masks[4] = {0, 0, 0, 0};

void calculate_com_masks(uint16_t number)
{
    static uint8_t segments[3];

    number %= 1000;  // Limit to 3 digits
    segments[2] = digit_segments[number / 100];
    segments[1] = digit_segments[(number / 10) % 10];
    segments[0] = digit_segments[number % 10];

    com_masks[0] = 0;
    com_masks[1] = 0;
    com_masks[2] = 0;
    com_masks[3] = 0;
    for (uint8_t i = 0; i < 3; i++)
    {
        uint8_t shift = i << 1;

        com_masks[0] += ((segments[i] >> 5) & 0x03) << shift;  // FA bits
        com_masks[1] += ((segments[i] >> 3) & 0x03) << shift;  // GB bits
        com_masks[2] += ((segments[i] >> 1) & 0x03) << shift;  // EC bits
        com_masks[3] += ((segments[i] << 1) & 0x03) << shift;  // D  bit
    }
}

void systick_init(void)
{
    SysTick->CTLR = 0;
    NVIC_EnableIRQ(SysTicK_IRQn);
    SysTick->CMP  = FUNCONF_SYSTEM_CORE_CLOCK / 1000 * 100 - 1;  // 100ms
    SysTick->CNT  = 0;
    SysTick->CTLR = SYSTICK_CTLR_STE | SYSTICK_CTLR_STIE | SYSTICK_CTLR_STCLK;
}

void SysTick_Handler(void) __attribute__((interrupt));
void SysTick_Handler(void)
{
    static uint16_t counter = 0;

    SysTick->CMP += FUNCONF_SYSTEM_CORE_CLOCK / 1000 * 100;  // 100ms
    SysTick->SR = 0;

    counter++;
    counter %= 1000;
    calculate_com_masks(counter);
}

int main(void)
{
    SystemInit();
    Delay_Ms(50);

    funGpioInitAll();
    funPinMode(PIN_COM1, GPIO_CNF_IN_FLOATING);
    funPinMode(PIN_COM2, GPIO_CNF_IN_FLOATING);
    funPinMode(PIN_COM3, GPIO_CNF_IN_FLOATING);
    funPinMode(PIN_COM4, GPIO_CNF_IN_FLOATING);
    funPinMode(PIN_SEG1, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP);
    funPinMode(PIN_SEG2, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP);
    funPinMode(PIN_SEG3, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP);
    funPinMode(PIN_SEG4, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP);
    funPinMode(PIN_SEG5, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP);
    funPinMode(PIN_SEG6, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP);
    funDigitalWrite(PIN_SEG1, FUN_LOW);
    funDigitalWrite(PIN_SEG2, FUN_LOW);
    funDigitalWrite(PIN_SEG3, FUN_LOW);
    funDigitalWrite(PIN_SEG4, FUN_LOW);
    funDigitalWrite(PIN_SEG5, FUN_LOW);
    funDigitalWrite(PIN_SEG6, FUN_LOW);

    systick_init();

    while (1)
    {
        // 1000ms / (2ms x 4) = 125 FPS
        for (uint8_t i = 0; i < 4; i++)
        {
            // COM - Output
            funPinMode(com_pins[i], GPIO_Speed_2MHz | GPIO_CNF_OUT_PP);
            // COM - High, SEG1-6 - Low as required
            funDigitalWrite(com_pins[i], FUN_HIGH);
            GPIOC->BSHR = (com_masks[i] << 18) + ((~com_masks[i] & 0x3F) << 2);
            Delay_Ms(1);
            // COM - Low, SEG1-6 - High as required
            funDigitalWrite(com_pins[i], FUN_LOW);
            GPIOC->BSHR = (com_masks[i] << 2) + ((~com_masks[i] & 0x3F) << 18);
            Delay_Ms(1);
            // COM - Float
            funPinMode(com_pins[i], GPIO_CNF_IN_FLOATING);
        }
    }
}
