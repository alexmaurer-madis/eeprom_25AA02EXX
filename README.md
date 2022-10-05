# 25AA02Exx 2k EEPROM Library (MIT License)


## Compatibility and dependencies
- Compatible with SPI EEPROM **25AA02E48** and **25AA02E64**.  
- Written for Arduino Framework.
- Depends on [Adafruit_BusIO Library](https://github.com/adafruit/Adafruit_BusIO) (MIT License)

---

## Asynchrone write operation

This library offers you the possibility to do asynchronous write operation while taking care of page boundaries.

- To be able to use the asynchronous function `begin_write` you must pass a buffer to the constructor.
- Choose a size for your buffer (up to 256) corresponding of your maximum data size you are willing to write in one call.
- Call the function `process()` in your loop.
- During the write operation, the function `is_write_in_progress()` return true.

```
#include "eeprom_25aa02exx.h"

uint8_t buffer[256];
EEPROM_25AA02EXX eeprom = EEPROM_25AA02EXX(buffer, 256);

void setup() {
  eeprom.begin_SPI(cs_pin, &SPI);
}

loop() {
  eeprom.process();
}
```


---
## EUI-48&trade; / EUI-64&trade;

>EUI-48™ and EUI-64™ are globally unique identification numbers standardized and provided by the IEEE Registration Authority.
>Both standards are composed of a 24-bit Organizationally Unique Identifier (OUI) followed by an Extension Identifier.
>
>Currently Microchip's OUIs assigned by the IEEE Registration Authority are : 
>- 0x0004A3
>- 0x001EC0
>- 0xD88039
>- 0x5410EC
>- though this will change as addresses are exhausted.

The 25AA02E48 is programmed at the factory with a 48-bit globally unique node address and the 25AA02E64 with a 64-bit one.  

You can use one of the following function to get the EUI corresponding of your EEPROM :
- `uint64_t read_EUI(EEPROM_25AA02EXX_EUI_Format::EUI48)`
- `uint64_t read_EUI(EEPROM_25AA02EXX_EUI_Format::EUI64)`

The EUI-48 node address of the 25AA02E48 can be encapsulated to form a globally unique 64-bit node :
- `uint64_t read_EUI(EEPROM_25AA02EXX_EUI_Format::EUI64_FROM_EUI48)`

The EUI-64&trade; location is in the upper 1/4 of the memory (FAh to FFh).
The remaining 1984 bits are available for application use.

[Have a look at the Microchip application note TB3187](https://ww1.microchip.com/downloads/en/Appnotes/TB3187-Organizationally-Unique-Identifiers-for-Preprogrammed-EUI-48-and-EUI-64-Address-Devices-90003187.pdf)



---
## Write protection - array protection bits BP1-BP0 (non-volatile)

The EEPROM is write protected with the external WP pin and the internal BP1-BP0 bits in Status Register.  


|  BP1  |  BP0  | Array addresses write-protected           | Function call                                        |
| :---: | :---: | ----------------------------------------- | ---------------------------------------------------- |
|   0   |   0   | none                                      | write_status(EEPROM_25AA02EXX_PROTECT_NONE)          |
|   0   |   1   | upper 1/4 (C0h to FFh) factory programmed | write_status(EEPROM_25AA02EXX_PROTECT_UPPER_QUARTER) |
|   1   |   0   | upper 1/2 (80h to FFh)                    | write_status(EEPROM_25AA02EXX_PROTECT_UPPER_HALF)    |
|   1   |   1   | all (00h to FFh)                          | write_status(EEPROM_25AA02EXX_PROTECT_ALL)           |



When using the function `simple_write(...)` don't forget to call `write_enable()` before each write attempt because the device reset the Write Latch after each write command.
<br />
<br />
The following function does not require to call write_enable :
- `write()`
- `begin_write()`
- `write_status()`

---
## Speed / timing
- SPI max clock frequency :
  - 5MHz @3V3 supply.
  - 10MHz @5V supply.
- 5ms Internal write cycle time (per page or byte).
