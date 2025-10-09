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
//    +---------+    |  F|   |B  |  PD0 COM1   1D   __   2D   __   3D   __
//    | D1 D2 D3|    |    -G-    |  PD6 COM2   1E   1C   2E   2C   3E   3C
//    +---------+    |  E|   |C  |  PD5 COM3   1G   1B   2G   2B   3G   3B
//   1 | | | | | 5   |    -D-    |  PD4 COM4   1F   1A   2F   2A   3F   3A
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

// Segments bit order: 0bDECGBFA
static const uint8_t character_segments[37] = {
    0b1110111,  // 0: ABCDEF_ = 1111110 -> 1110111
    0b0010100,  // 1: _BC____ = 0110000 -> 0010100
    0b1101101,  // 2: AB_DE_G = 1101101 -> 1101101
    0b1011101,  // 3: ABCD__G = 1111001 -> 1011101
    0b0011110,  // 4: _BC__FG = 0110011 -> 0011110
    0b1011011,  // 5: A_CD_FG = 1011011 -> 1011011
    0b1111011,  // 6: A_CDEFG = 1011111 -> 1111011
    0b0010101,  // 7: ABC____ = 1110000 -> 0010101
    0b1111111,  // 8: ABCDEFG = 1111111 -> 1111111
    0b1011111,  // 9: ABCD_FG = 1111011 -> 1011111
    0b0111111,  // A: ABC_EFG = 111_111 -> 0111111
    0b1111010,  // b: __CDEFG = 0011111 -> 1111010
    0b1100011,  // C: A__DEF_ = 1001110 -> 1100011
    0b1111100,  // d: _BCDE_G = 0111101 -> 1111100
    0b1101011,  // E: A__DEFG = 1001111 -> 1101011
    0b0101011,  // F: A___EFG = 1000111 -> 0101011
    0b1110011,  // G: A_CDEF_ = 1011110 -> 1110011
    0b0111110,  // H: _BC_EFG = 0110111 -> 0111110
    0b0100010,  // I: ____EF_ = 0000110 -> 0100010
    0b1010100,  // J: _BCD___ = 0111000 -> 1010100
    0b0111011,  // K: A_C_EFG = 1010111 -> 0111011
    0b1100010,  // L: ___DEF_ = 0001110 -> 1100010
    0b1000111,  // M: AB_D_F_ = 1101010 -> 1000111
    0b0110111,  // N: ABC_EF_ = 1110110 -> 0110111
    0b1111000,  // o: __CDE_G = 0011101 -> 1111000
    0b0101111,  // P: AB__EFG = 1100111 -> 0101111
    0b0011111,  // q: ABC__FG = 1110011 -> 0011111
    0b0101000,  // r: ____E_G = 0000101 -> 0101000
    0b1011011,  // S: A_CD_FG = 1011011 -> 1011011
    0b1101010,  // t: ___DEFG = 0001111 -> 1101010
    0b1110110,  // U: _BCDEF_ = 0111110 -> 1110110
    0b1010110,  // V: _BCD_F_ = 0111010 -> 1010110
    0b1110001,  // W: A_CDE__ = 1011100 -> 1110001
    0b1001000,  // x: ___D__G = 0001001 -> 1001000
    0b1011110,  // y: _BCD_FG = 0111011 -> 1011110
    0b1101101,  // z: AB_DE_G = 1101101 -> 1101101
    0b0000000,  // (space)              -> 0000000
};

static const uint8_t com_pins[4]  = {PIN_COM1, PIN_COM2, PIN_COM3, PIN_COM4};
volatile uint8_t     seg_masks[4] = {0, 0, 0, 0};

void calculate_seg_masks(const uint8_t d1_segs, const uint8_t d2_segs, const uint8_t d3_segs)
{
    // Convert Character Bit Order (0bDECGBFA) to Segment Masks for Each Common Pin
    //                         LCD Char | Seg Group Mask   | <<Shifts>>
    //                         D1 D2 D3 | 0bDECGBFA        | D1  D2  D3
    // Segment Mask for COM1 - D_ D_ D_ - 0b1000000 / 0x40 - >>1 >>3 >>5
    // Segment Mask for COM2 - EC EC EC - 0b0110000 / 0x30 -     >>2 >>4
    // Segment Mask for COM3 - GB GB GB - 0b0001100 / 0x0C - <<2     >>2
    // Segment Mask for COM4 - FA FA FA - 0b0000011 / 0x03 - <<4 <<2
    //              Segments - 65 43 21
    seg_masks[0] = ((d1_segs & 0x40) >> 1) | ((d2_segs & 0x40) >> 3) | ((d3_segs & 0x40) >> 5);  // COM1: D  bits
    seg_masks[1] = ((d1_segs & 0x30) >> 0) | ((d2_segs & 0x30) >> 2) | ((d3_segs & 0x30) >> 4);  // COM2: EC bits
    seg_masks[2] = ((d1_segs & 0x0C) << 2) | ((d2_segs & 0x0C) >> 0) | ((d3_segs & 0x0C) >> 2);  // COM3: GB bits
    seg_masks[3] = ((d1_segs & 0x03) << 4) | ((d2_segs & 0x03) << 2) | ((d3_segs & 0x03) >> 0);  // COM4: FA bits
}

void show_hex_number(uint16_t number)
{
    calculate_seg_masks(character_segments[(number >> 8) & 0x0F],  // D1
                        character_segments[(number >> 4) & 0x0F],  // D2
                        character_segments[number & 0x0F]);        // D3
}

void show_string(const char* str)
{
    uint8_t segs[3] = {0, 0, 0};  // D1 D2 D3

    for (uint8_t i = 0; i < 3; i++)
    {
        char c = str[i];
        if (c == '\0')
            break;  // Early termination

        uint8_t index;
        if (c >= '0' && c <= '9')
        {
            index = c - '0';
        }
        else if (c == ' ')
        {
            index = 36;
        }
        else
        {
            c |= 0x20;  // Case-insensitive
            if (c >= 'a' && c <= 'z')
            {
                index = c - 'a' + 10;
            }
            else
            {
                continue;  // Skip invalid characters
            }
        }

        segs[i] = character_segments[index];
    }

    calculate_seg_masks(segs[0], segs[1], segs[2]);
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
    // LCDReady  3  2  1  0 Go
    // 01234567890123456789012
    static char*    startup = "LCDReady  3  2  1  0 Go";
    static uint16_t counter = 0;

    SysTick->CMP += FUNCONF_SYSTEM_CORE_CLOCK / 1000 * 100;  // 100ms
    SysTick->SR = 0;

    if (++counter < 64)
        show_string(&startup[(counter >> 3) * 3]);
    else if (counter > 0xFFF + 64)
        counter = 0;
    else
        show_hex_number(counter - 64);
}

int main(void)
{
    SystemInit();

    funGpioInitAll();
    // funPinMode(PIN_COM1, GPIO_CNF_IN_FLOATING);
    // funPinMode(PIN_COM2, GPIO_CNF_IN_FLOATING);
    // funPinMode(PIN_COM3, GPIO_CNF_IN_FLOATING);
    // funPinMode(PIN_COM4, GPIO_CNF_IN_FLOATING);
    // funPinMode(PIN_SEG1, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP);
    // funPinMode(PIN_SEG2, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP);
    // funPinMode(PIN_SEG3, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP);
    // funPinMode(PIN_SEG4, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP);
    // funPinMode(PIN_SEG5, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP);
    // funPinMode(PIN_SEG6, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP);
    // funDigitalWrite(PIN_SEG1, FUN_LOW);
    // funDigitalWrite(PIN_SEG2, FUN_LOW);
    // funDigitalWrite(PIN_SEG3, FUN_LOW);
    // funDigitalWrite(PIN_SEG4, FUN_LOW);
    // funDigitalWrite(PIN_SEG5, FUN_LOW);
    // funDigitalWrite(PIN_SEG6, FUN_LOW);

    GPIOD->CFGLR = (GPIOD->CFGLR & 0xF000FFF0) | 0x04440004;  // Set PD0, PD4, PD5, PD6 to floating input
    GPIOC->CFGLR = (GPIOC->CFGLR & 0xFF000000) | 0x00222222;  // Set PC0-PC5 to 2MHz push-pull output
    GPIOC->BSHR  = 0x3F << 16;                                // Set PC0-PC5 to LOW

    systick_init();

    while (1)
    {
        // 1000ms / (4ms x 4) = 62.5 FPS
        for (uint8_t i = 0; i < 4; i++)
        {
            const uint8_t com_pin      = com_pins[i];
            const uint8_t seg_mask     = seg_masks[i];
            const uint8_t inv_seg_mask = ~seg_mask & 0x3F;  // Keep lower 6 bits for PC5-PC0

            // COM - Output
            funPinMode(com_pin, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP);

            // COM - High, SEG1-6 - Low as required
            funDigitalWrite(com_pin, FUN_HIGH);
            GPIOC->BSHR = (seg_mask << 16) | inv_seg_mask;
            Delay_Ms(2);

            // COM - Low, SEG1-6 - High as required
            funDigitalWrite(com_pin, FUN_LOW);
            GPIOC->BSHR = (inv_seg_mask << 16) | seg_mask;
            Delay_Ms(2);

            // COM - Float
            funPinMode(com_pin, GPIO_CNF_IN_FLOATING);
        }
    }
}
