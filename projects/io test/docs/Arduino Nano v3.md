# Arduino Nano v3 board

## Pinout

This is a simplified pinout of the board. Full pinout can be found [here](https://docs.arduino.cc/resources/pinouts/A000005-full-pinout.pdf).

```txt
                            USB Type Mini-B
                                ╔═══════╗
                 ╔══════════════╣███████╠═════════════╗
                 ║              ║███████║             ║
                 ║              ║███████║             ║
  SCK | PB5 ---> ║ [◼] D13     ╚═══════╝     D12 [◼] ║ <--- PB4       | CIPO
                 ║ [◼] +3.3V   ┌────────┐    D11 [◼] ║ <--- PB3 | PWM | COPI
                 ║ [◼] REF     │ CH340C │    D10 [◼] ║ <--- PB2 | PWM | SS
        PC0 ---> ║ [◼] A0      └────────┘    D9  [◼] ║ <--- PB1 | PWM
        PC1 ---> ║ [◼] A1      ┏━━━━━━━━┓    D8  [◼] ║ <--- PB0 | PWM
        PC2 ---> ║ [◼] A2      ┃ ATMEGA ┃    D7  [◼] ║ <--- PD7
        PC3 ---> ║ [◼] A3      ┃  328P  ┃    D6  [◼] ║ <--- PD6 | PWM
  SDA | PC4 ---> ║ [◼] A4      ┗━━━━━━━━┛    D5  [◼] ║ <--- PD5 | PWM
  SCL | PC5 ---> ║ [◼] A5     ╔══════════╗   D4  [◼] ║ <--- PD4 | PWM
                 ║ [◼] A6     ║ ████████ ║   D3  [◼] ║ <--- PD3 | PWM | INT1
                 ║ [◼] A7     ╚══════════╝   D2  [◼] ║ <--- PD2       | INT0
                 ║ [◼] +5V    RESET BUTTON   GND [◼] ║
RESET | PC6 ---> ║ [◼] RST  ┌─┐ ┌─┐ ┌─┐ ┌─┐  RST [◼] ║ <--- PC6 | RESET
                 ║ [◼] GND  │█│ │█│ │█│ │█│  RX0 [◼] ║ <--- PD0 | RX
                 ║ [◼] VIN  └─┘ └─┘ └─┘ └─┘  TX1 [◼] ║ <--- PD1 | TX
                 ║           L  PWR  TX  RX           ║
                 ╚════════════════════════════════════╝
                             ↑
                            PB5 | LED_BUILTIN
```
