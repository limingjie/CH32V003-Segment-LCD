# CH32V003 Segment LCD

Efficient segment LCD driver for the low-cost MCU CH32V003.

![Breadboard](./Images/CH32V003%20Segment%20LCD%20-%20Breadboard.jpeg)

## TN Positive 3-Digit Segment LCD Panel

I found these LCD panels on [AliExpress](https://www.aliexpress.com/item/1005005697772193.html).

### Specifications

10-Pin TN Positive 3-Digit Segment LCD Panel, 3.0V Digital Tube Display

1. **Dimensions**: 11.5×10.3/6.3×2 mm, 10 pins
2. **Conductive glass thickness**: 0.7 mm
3. **Minimum pixel gap**: 10 μm
4. **Duty ratio**: 1/4 duty, 1/2 bias
5. **Voltage**: 3.0 V
6. **Operating temperature**: 0°C to +50°C
7. **Viewing direction**: 6 o'clock
8. **Connection method**: Metal pin
9. **TN type viewing range**: 60°, reflective

![LCD Panel Dimension](./Images/LCD%20Panel%20Dimension.png)

### LCD Multiplex

- Each Common pin controls 1 or 2 segments across 3 characters.
- Each Segment pin controls 3 or 4 segments within 1 character.


![LCD Bit Mapping](./Images/LCD%20Bit%20Mapping.png)

The wiring diagram of the LCD display (from [Pacific Display](https://www.pacificdisplay.com/lcd_multiplex_drive.htm)).

![LCD Multiplex](./Images/Pacific%20Display%20LCD%20Multiplex.gif)

### LCD Timing Diagram

LCDs use AC signals to prevent damage and ensure longevity by avoiding DC-induced charge accumulation, which can permanently polarize and degrade the liquid crystal molecules.

For a 1/2 bias LCD, the Common pins require 3 voltages: +V, +V/2, and 0. Use the following timing sequence to ensure the average voltage across the LCD is 0.

![LCD Timing Diagram](./Charts/LCD%20Timing%20Diagram.png)

### LCD Characters

![LCD Characters](./Images/LCD%20Characters.png)

## References

- [EEVblog #1045 - How To Drive an LCD](https://www.youtube.com/watch?v=ZP0KxZl5N2o)
- [awawa's lab - Direct LCD drive by AVR GPIO](https://awawa.hariko.com/avr_lcd_drive_en.html)
- [LEAP: Little Electronic Art Projects #663 3-Digit LCD Panel](https://github.com/tardate/LittleArduinoProjects/tree/main/Electronics101/Displays/Lcd3Digit)

## License

![CC by-nc-sa](Images/by-nc-sa.svg)

This work is licensed under a [Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License (CC BY-NC-SA 4.0)](https://creativecommons.org/licenses/by-nc-sa/4.0/).

**You are free to:**

- **Share** — copy and redistribute the material in any medium or format
- **Adapt** — remix, transform, and build upon the material

The licensor cannot revoke these freedoms as long as you follow the license terms.

**Under the following terms:**

- **Attribution** - You must give appropriate credit, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
- **NonCommercial** - You may not use the material for commercial purposes.
- **ShareAlike** - If you remix, transform, or build upon the material, you must distribute your contributions under the same license as the original.
- **No additional restrictions** — You may not apply legal terms or technological measures that legally restrict others from doing anything the license permits.

**Notices:**

You do not have to comply with the license for elements of the material in the public domain or where your use is permitted by an applicable exception or limitation.

No warranties are given. The license may not give you all of the permissions necessary for your intended use. For example, other rights such as publicity, privacy, or moral rights may limit how you use the material.
