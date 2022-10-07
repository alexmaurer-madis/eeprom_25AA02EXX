/*!
 * @file eeprom_25AA02EXX.h
 */

#ifndef __ALEXMAURER_EEPROM_25AA02EXX_H__
#define __ALEXMAURER_EEPROM_25AA02EXX_H__

#include <Adafruit_BusIO_Register.h>
#include <Adafruit_SPIDevice.h>
#include <Arduino.h>

#define EEPROM_25AA02EXX_MEMORY_SIZE 256
#define EEPROM_25AA02EXX_PAGE_SIZE 16

// INSTRUCTION SET

// Read data from memory array beginning at selected address
#define EEPROM_25AA02EXX_READ 0b011
// Write date to memory array beginning at selected address
#define EEPROM_25AA02EXX_WRITE 0b010
// Reset the write enable latch (disable write operations)
#define EEPROM_25AA02EXX_WRDI 0b100
// Set the write enable latch (enable write operations)
#define EEPROM_25AA02EXX_WREN 0b110
// Read STATUS register
#define EEPROM_25AA02EXX_RDSR 0b101
// Write STATUS register
#define EEPROM_25AA02EXX_WRSR 0b001

// STATUS REGISTER bit BP1-BP0 (RW), indicate which blocks are currently
// write-protected
#define EEPROM_25AA02EXX_BP1 0b1000
// STATUS REGISTER bit BP1-BP0 (RW), indicate which blocks are currently
// write-protected
#define EEPROM_25AA02EXX_BP0 0b0100
// STATUS REGISTER bit WEL (RO), Write Enable Latch. When set to 1, the latch
// allow writes to the array
#define EEPROM_25AA02EXX_WEL 0b0010
// STATUS REGISTER bit WIP (RO), Write In Progress bit (1=a write is in
// progress)
#define EEPROM_25AA02EXX_WIP 0b0001

// BP1-BP0 bits are set with WRSR instruction and are nonvolatile
// Factory-programmed Write protection (BP1=0, BP0=1).
// This protects the upper 1/4 of the array (0xC0 to 0xFF)

// Entire array of memory is writable
#define EEPROM_25AA02EXX_PROTECT_NONE 0b0000
// Upper quarter is protected C0h to FFh
#define EEPROM_25AA02EXX_PROTECT_UPPER_QUARTER 0b0100
// Upper half is protected 80h to FFh
#define EEPROM_25AA02EXX_PROTECT_UPPER_HALF 0b1000
// Entier array is protected
#define EEPROM_25AA02EXX_PROTECT_ALL 0b1100

enum class EEPROM_25AA02EXX_EUI_Format {
  EUI48,
  EUI64_FROM_EUI48,
  EUI64,
};

class EEPROM_25AA02EXX {
 public:
  EEPROM_25AA02EXX();
  EEPROM_25AA02EXX(uint8_t *buffer, size_t writeBufferSize);

  bool begin_SPI(uint8_t cs_pin, SPIClass *theSPI = &SPI);
  bool begin_SPI(uint8_t cs_pin, uint32_t freq, SPIClass *theSPI = &SPI);

  uint8_t read_status();
  void write_status(uint8_t bitProtection);

  bool write_enable(void);
  bool write_disable(void);

  bool is_write_in_progress(void);

  uint8_t read(uint8_t address);
  void read(uint8_t address, uint8_t *dst, size_t len);

  uint64_t read_EUI(EEPROM_25AA02EXX_EUI_Format format);

  void simple_write(uint8_t address, uint8_t *src, size_t len);
  void write(uint8_t address, uint8_t *src, size_t len);
  void begin_write(uint8_t address, uint8_t *src, size_t len);

  void process(void);

 protected:
  Adafruit_SPIDevice *spi_dev = NULL;  ///< Pointer to SPI bus interface

 private:
  uint8_t *_writeBuffer = nullptr;
  size_t _writeBufferSize = 0;

  void _write_prepare(uint8_t address, uint8_t *src, size_t len);
  void _next_write(void);

  volatile bool _writeBegin = false;

  struct _writeOperationStruct {
    uint8_t *src;
    uint8_t currentAddress;
    uint8_t pageStart;
    uint8_t pageEnd;
    uint8_t totalPages;
    uint8_t currentPage;
    size_t lenToWrite;
    unsigned long lastWIPCheck;
  } _wo;
};

#endif