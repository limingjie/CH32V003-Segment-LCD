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
//    LCD PINOUT     |  Segments |  Segment Matrix and CH32V003 Pin Mapping
//                   |           |
//  COM1 2 3 4 SEG6  |           |             PC5  PC4  PC3  PC2  PC1  PC0
//  10 | | | | | 6   |    -A-    |            SEG6 SEG5 SEG4 SEG3 SEG2 SEG1
//    +---------+    |  F|   |B  |  PD4 COM4   1F   1A   2F   2A   3F   3A
//    | D1 D2 D3|    |    -G-    |  PD5 COM3   1G   1B   2G   2B   3G   3B
//    +---------+    |  E|   |C  |  PD6 COM2   1E   1C   2E   2C   3E   3C
//   1 | | | | | 5   |    -D-    |  PD0 COM1   1D   __   2D   __   3D   __
//  SEG1 2 3 4 5     |           |

#define PIN_COM4 PD4
#define PIN_COM3 PD5
#define PIN_COM2 PD6
#define PIN_COM1 PD0
#define PIN_SEG6 PC5
#define PIN_SEG5 PC4
#define PIN_SEG4 PC3
#define PIN_SEG3 PC2
#define PIN_SEG2 PC1
#define PIN_SEG1 PC0

// Segments bit order: 0bFAGBECD
static const uint8_t digit_segments[37] = {
    0b1101111,  // 0: ABCDEF_ = 1111110 -> 1101111
    0b0001010,  // 1: _BC____ = 0110000 -> 0001010
    0b0111101,  // 2: AB_DE_G = 1101101 -> 0111101
    0b0111011,  // 3: ABCD__G = 1111001 -> 0111011
    0b1011010,  // 4: _BC__FG = 0110011 -> 1011010
    0b1110011,  // 5: A_CD_FG = 1011011 -> 1110011
    0b1110111,  // 6: A_CDEFG = 1011111 -> 1110111
    0b0101010,  // 7: ABC____ = 1110000 -> 0101010
    0b1111111,  // 8: ABCDEFG = 1111111 -> 1111111
    0b1111011,  // 9: ABCD_FG = 1111011 -> 1111011
    0b0000000,  // (space)
    0b1111110,  // A: ABC_EFG = 111_111 -> 1111110
    0b1010111,  // b: __CDEFG = 0011111 -> 1010111
    0b1100101,  // C: A__DEF_ = 1001110 -> 1100101
    0b0011111,  // d: _BCDE_G = 0111101 -> 0011111
    0b1110101,  // E: A__DEFG = 1001111 -> 1110101
    0b1110100,  // F: A___EFG = 1000111 -> 1110100
    0b1100111,  // G: A_CDEF_ = 1011110 -> 1100111
    0b1011110,  // H: _BC_EFG = 0110111 -> 1011110
    0b1000100,  // I: ____EF_ = 0000110 -> 1000100
    0b0001011,  // J: _BCD___ = 0111000 -> 0001011
    0b1110110,  // K: A_C_EFG = 1010111 -> 1110110
    0b1000101,  // L: ___DEF_ = 0001110 -> 1000101
    0b1101001,  // M: AB_D_F_ = 1101010 -> 1101001
    0b1101110,  // N: ABC_EF_ = 1110110 -> 1101110
    0b0010111,  // o: __CDE_G = 0011101 -> 0010111
    0b1111100,  // P: AB__EFG = 1100111 -> 1111100
    0b1111010,  // q: ABC__FG = 1110011 -> 1111010
    0b0010100,  // r: ____E_G = 0000101 -> 0010100
    0b1110011,  // S: A_CD_FG = 1011011 -> 1110011
    0b1010101,  // t: ___DEFG = 0001111 -> 1010101
    0b1001111,  // U: _BCDEF_ = 0111110 -> 1001111
    0b1001011,  // V: _BCD_F_ = 0111010 -> 1001011
    0b0100111,  // W: A_CDE__ = 1011100 -> 0100111
    0b0010001,  // x: ___D__G = 0001001 -> 0010001
    0b1011011,  // y: _BCD_FG = 0111011 -> 1011011
    0b0111101,  // z: AB_DE_G = 1101101 -> 0111101
};

static const uint8_t com_pins[4]  = {PIN_COM4, PIN_COM3, PIN_COM2, PIN_COM1};
volatile uint8_t     com_masks[4] = {0, 0, 0, 0};

void calculate_com_masks(uint8_t d1_segs, uint8_t d2_segs, uint8_t d3_segs)
{
    // Convert from 0bFAGBECD to com_masks
    //       D1 D2 D3
    //    0b xx xx xx
    // COM4: FA FA FA
    // COM3: GB GB GB
    // COM2: EC EC EC
    // COM1: D0 D0 D0
    com_masks[0] = ((d1_segs & 0x60) >> 1) | ((d2_segs & 0x60) >> 3) | ((d3_segs & 0x60) >> 5);  // COM4: FA bits
    com_masks[1] = ((d1_segs & 0x18) << 1) | ((d2_segs & 0x18) >> 1) | ((d3_segs & 0x18) >> 3);  // COM3: GB bits
    com_masks[2] = ((d1_segs & 0x06) << 3) | ((d2_segs & 0x06) << 1) | ((d3_segs & 0x06) >> 1);  // COM2: EC bits
    com_masks[3] = ((d1_segs & 0x01) << 5) | ((d2_segs & 0x01) << 3) | ((d3_segs & 0x01) << 1);  // COM1: D  bits
}

void show_number(uint16_t number, uint8_t base)
{
    uint8_t d3_segs = digit_segments[number % base];  // D3
    number /= base;
    uint8_t d2_segs = digit_segments[number % base];  // D2
    number /= base;
    uint8_t d1_segs = digit_segments[number % base];  // D1

    calculate_com_masks(d1_segs, d2_segs, d3_segs);
}

void show_string(const char* str)
{
    uint8_t segs[3] = {0, 0, 0};  // D1, D2, D3

    for (int i = 0; i < 3; i++)
    {
        char c = str[i];

        if (c >= '0' && c <= '9')
            segs[i] = digit_segments[c - '0'];
        else if (c == ' ')
            segs[i] = digit_segments[10];
        else if (c >= 'A' && c <= 'Z')
            segs[i] = digit_segments[c - 'A' + 11];
        else if (c >= 'a' && c <= 'z')
            segs[i] = digit_segments[c - 'a' + 11];
    }

    calculate_com_masks(segs[0], segs[1], segs[2]);
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
    static int16_t counter = -80;

    SysTick->CMP += FUNCONF_SYSTEM_CORE_CLOCK / 1000 * 100;  // 100ms
    SysTick->SR = 0;

    counter = (counter + 1) % 1000;

    if (counter < -70)
        show_string("LCD");
    else if (counter < -60)
        show_string("Rea");
    else if (counter < -50)
        show_string("dy ");
    else if (counter < -40)
        show_string(" 3 ");
    else if (counter < -30)
        show_string(" 2 ");
    else if (counter < -20)
        show_string(" 1 ");
    else if (counter < -10)
        show_string(" 0 ");
    else if (counter < 0)
        show_string("Go ");
    else
        show_number(counter, 10);
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
    // funDigitalWrite(PIN_SEG1, FUN_LOW);
    // funDigitalWrite(PIN_SEG2, FUN_LOW);
    // funDigitalWrite(PIN_SEG3, FUN_LOW);
    // funDigitalWrite(PIN_SEG4, FUN_LOW);
    // funDigitalWrite(PIN_SEG5, FUN_LOW);
    // funDigitalWrite(PIN_SEG6, FUN_LOW);
    GPIOC->BSHR = 0x3F << 16;  // Reset PC0-PC5 to LOW

    systick_init();

    while (1)
    {
        // 1000ms / (4ms x 4) = 62.5 FPS
        for (uint8_t i = 0; i < 4; i++)
        {
            uint8_t mask     = com_masks[i];
            uint8_t inv_mask = ~mask & 0x3F;  // Keep lower 6 bits for PC0-PC5
            uint8_t com_pin  = com_pins[i];

            // COM - Output
            funPinMode(com_pin, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP);

            // COM - High, SEG1-6 - Low as required
            funDigitalWrite(com_pin, FUN_HIGH);
            GPIOC->BSHR = (mask << 16) | inv_mask;
            Delay_Ms(2);

            // COM - Low, SEG1-6 - High as required
            funDigitalWrite(com_pin, FUN_LOW);
            GPIOC->BSHR = mask | (inv_mask << 16);
            Delay_Ms(2);

            // COM - Float
            funPinMode(com_pin, GPIO_CNF_IN_FLOATING);
        }
    }
}
