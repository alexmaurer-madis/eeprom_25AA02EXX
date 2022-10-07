# 25AA02Exx 2k Microchip SPI EEPROM Library (MIT License)

Current master branch is still in beta.

Some examples will be soon published.

## Compatibility and dependencies
- Compatible with SPI EEPROM **25AA02E48** and **25AA02E64**.  
- Written for Arduino Framework.
- Depends on [Adafruit_BusIO Library](https://github.com/adafruit/Adafruit_BusIO) (MIT License)

## Synchronous write operation aka "blocking function"
The ```write()``` function is programmed in such a way as to respect the paging of the memory (page boundaries).
You can then easily write to any memory location without worrying about going over a page.

This function is blocking during all the write cycle(s). If you cannot afford to wait that amount of time, you can
use the ```begin_write()``` asynchronous function.

## Asynchrone write operation aka "non-blocking function"
This library offers you the possibility to do asynchronous write operation while taking care of page boundaries.  
(as the ```write()``` function does already).

- To be able to use the asynchronous function `begin_write` you must pass a buffer to the constructor.
- Choose a size up to 256 corresponding of your maximum data size you are willing to write in one call.
- In your loop(), add a call to the function `process()`.
- The function `is_write_in_progress()` return true while a write is in progress.

```
#include "eeprom_25aa02exx.h"

#define EEPROM_CS_PIN 2

uint8_t buffer[256];
EEPROM_25AA02EXX eeprom = EEPROM_25AA02EXX(buffer, 256);

void setup() {
  eeprom.begin_SPI(EEPROM_CS_PIN);
}

loop() {
  eeprom.process();
}
```

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


## Write protection - array protection bits BP1-BP0

The EEPROM can be write protected with the external WP pin and the internal bits BP1-BP0 (in Status Register).  
At factory, the upper 1/4 of the array (0xC0 to 0xFF) is write protected (where resides the EUI-48 or EUI-64).

**BP1-BP0 bits are non-volatile.**

|  BP1  |  BP0  | Array addresses write-protected           | Function to call                                     |
| :---: | :---: | ----------------------------------------- | ---------------------------------------------------- |
|   0   |   0   | none                                      | write_status(EEPROM_25AA02EXX_PROTECT_NONE)          |
|   0   |   1   | upper 1/4 (C0h to FFh) factory programmed | write_status(EEPROM_25AA02EXX_PROTECT_UPPER_QUARTER) |
|   1   |   0   | upper 1/2 (80h to FFh)                    | write_status(EEPROM_25AA02EXX_PROTECT_UPPER_HALF)    |
|   1   |   1   | all (00h to FFh)                          | write_status(EEPROM_25AA02EXX_PROTECT_ALL)           |


## write_enable() / write_disable()

When using the function `simple_write(...)` (which is not blocking and which does not manage pagination), don't forget to call `write_enable()` before each write attempt because the device resets the Write Latch after each write command.  

The following functions does not require callong `write_enable()` :
- `write_status()`
- `write()`
- `begin_write()`

## Speed and timing
- SPI max clock frequency :
  - 5MHz @3V3 supply (by default if not specified in the constructor).
  - 10MHz @5V supply.
- Device limitation of 5ms internal write cycle time.
