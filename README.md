# CH32V003 Segment LCD

Efficient Segment LCD driver by the low-cost MCU CH32V003.

![Breadboard](./Images/CH32V003%20Segment%20LCD%20-%20Breadboard.jpeg)

## TN Positive 3-Digits Segment LCD Panel

I found these LCD panels from [AliExpress](https://www.aliexpress.com/item/1005005697772193.html).

### Specification

10PIN TN Positive 3-Digits Segment LCD Panel 3.0V Digital Tube Display

1. **Dimensions**: 11.5x10.3/6.3x2MM, 10 pins.
2. **Conductive glass thickness**: 0.7mm
3. **Minimum pixel gap**: 10um
4. **Duty ratio**: 1/4duty 1/2BIAS
5. **Voltage**: 3.0V
6. **Working temperature**: -0 degrees to +50 degrees
7. **Viewing direction**: 6 points
8. **Connection method**: metal pin
9. TN type viewing range 60 degrees. Reflection.

![LCD Panel Dimension](./Images/LCD%20Panel%20Dimension.png)

### LCD Multiplex

![LCd Panel](./Images/LCD%20Panel.png)

It is the same as the [Glass LCD Products from Pacific Display](https://www.pacificdisplay.com/lcd_multiplex_drive.htm).

![LCD Multiplex](./Images/Pacific%20Display%20LCD%20Multiplex.gif)

|          | SEG6  | SEG5  | SEG4  | SEG3  | SEG2  | SEG1  |
| :------: | :---: | :---: | :---: | :---: | :---: | :---: |
| **COM4** |  1F   |  1A   |  2F   |  2A   |  3F   |  3A   |
| **COM3** |  1G   |  1B   |  2G   |  2B   |  3G   |  3B   |
| **COM2** |  1E   |  1C   |  2E   |  2C   |  3E   |  3C   |
| **COM1** |  1D   |       |  2D   |       |  3D   |       |

### LCD Timing Diagram

![LCD Timing Diagram](./Charts/LCD%20Timing%20Diagram.svg)

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
