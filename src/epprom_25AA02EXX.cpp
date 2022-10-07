#include "eeprom_25AA02EXX.h"

/**
 * @brief Construct a new eeprom 25AA02EXX object without buffer (no
 * asynchronous operation)
 *
 */
EEPROM_25AA02EXX::EEPROM_25AA02EXX() { _writeBufferSize = 0; }

/**
 * @brief Construct a new eeprom 25AA02EXX object. Specify your buffer for
 * asynchronous write operations. Choose a size (up to 256) corresponding of
 * your maximum data size you are willing to write in one call.
 *
 * @param writeBufferSize size of your buffer
 * @param buffer pointer to your buffer
 */
EEPROM_25AA02EXX::EEPROM_25AA02EXX(uint8_t *buffer,
                                   const size_t writeBufferSize) {
  _writeBuffer = buffer;
  _writeBufferSize = writeBufferSize;
}

/**
 * @brief Initialize using hardware SPI. Clock frequency 5MHz for 3V3 operation.
 *
 * @param cs_pin Pin to use for SPI chip select
 * @param theSPI Pointer to SPI instance
 * @return true if initialization successful, otherwise false.
 */
bool EEPROM_25AA02EXX::begin_SPI(uint8_t cs_pin, SPIClass *theSPI) {
  spi_dev = new Adafruit_SPIDevice(cs_pin, 5000000, SPI_BITORDER_MSBFIRST,
                                   SPI_MODE0, theSPI);
  return spi_dev->begin();
}

/**
 * @brief Initialize using hardware SPI.
 *
 * @param cs_pin Pin to use for SPI chip select
 * @param freq SPI Clock frequency in Hz (max 10MHz)
 * @param theSPI Pointer to SPI instance
 * @return true if initialization successful, otherwise false.
 */
bool EEPROM_25AA02EXX::begin_SPI(uint8_t cs_pin, uint32_t freq,
                                 SPIClass *theSPI) {
  assert(freq <= 10000000);
  spi_dev = new Adafruit_SPIDevice(cs_pin, freq, SPI_BITORDER_MSBFIRST,
                                   SPI_MODE0, theSPI);
  return spi_dev->begin();
}

/**
 * @brief Enable Write Latch and check Status register
 *
 * @return bool Return true when WriteEnable Latch is set
 */
bool EEPROM_25AA02EXX::write_enable() {
  const uint8_t request[1] = {EEPROM_25AA02EXX_WREN};
  spi_dev->write(request, 1);

  return ((read_status() & EEPROM_25AA02EXX_WEL) != 0) ? true : false;
}

/**
 * @brief Reset Write Latch and check Status register
 *
 * @return bool Return true when WriteEnable Latch is reset
 */
bool EEPROM_25AA02EXX::write_disable() {
  const uint8_t request[1] = {EEPROM_25AA02EXX_WRDI};
  spi_dev->write(request, 1);

  return ((read_status() & EEPROM_25AA02EXX_WEL) != 0) ? false : true;
}

/**
 * @brief Read status register
 *
 * @return uint8_t status register
 */
uint8_t EEPROM_25AA02EXX::read_status() {
  const uint8_t request[2] = {EEPROM_25AA02EXX_RDSR};
  uint8_t status;
  spi_dev->write_then_read(request, 1, &status, 1);

  return status;
}

/**
 * @brief Write status register. This function is blocking. write_enable() is
 * automatically called.
 *
 * @return uint8_t status register
 */
void EEPROM_25AA02EXX::write_status(const uint8_t bitProtection) {
  write_enable();
  const uint8_t request[2] = {EEPROM_25AA02EXX_WRSR, bitProtection};
  spi_dev->write(request, 2);

  // Write cycle takes approx. 5ms
  while (read_status() & EEPROM_25AA02EXX_WIP) {
    delay(1);
  }
}

/**
 * @brief Read one byte of data
 *
 * @param address
 * @return uint8_t
 */
uint8_t EEPROM_25AA02EXX::read(const uint8_t address) {
  const uint8_t request[2] = {EEPROM_25AA02EXX_READ, address};
  uint8_t data;
  spi_dev->write_then_read(request, 2, &data, 1);

  return data;
}

/**
 * @brief Read one or many bytes into your buffer
 *
 * @param address Starting address to read from
 * @param dst Destination buffer to write to
 * @param len Len of data to read
 */
void EEPROM_25AA02EXX::read(const uint8_t address, uint8_t *dst,
                            const size_t len) {
  const uint8_t request[2] = {EEPROM_25AA02EXX_READ, address};
  spi_dev->write_then_read(request, 2, dst, len);

  return;
}

/**
 * @brief Read the Node Identity
 *
 * @param format Format of EUI to read
 * @return uint64_t Return the EUI-48 or EUI-64
 */
uint64_t EEPROM_25AA02EXX::read_EUI(EEPROM_25AA02EXX_EUI_Format format) {
  // Node address is stored big endian
  // EUI-48 stored from FAh to FFh (can be encapsulated in a EUI-64)
  // EUI-64 stored from F8h to FFh

  uint8_t EUI64_buffer[8];
  read(0xF8, EUI64_buffer, 8);

  uint64_t EUI_value = 0;
  for (int i = (format == EEPROM_25AA02EXX_EUI_Format::EUI48) ? 2 : 0; i <= 7;
       i++) {
    EUI_value <<= 8;
    EUI_value |= (uint64_t)EUI64_buffer[i];
  }

  // EUI-48 to EUI-64 Encapsulation if desired
  if (format == EEPROM_25AA02EXX_EUI_Format::EUI64_FROM_EUI48) {
    uint64_t encapsulation =
        EUI_value & 0x0000FFFFFF000000;   // Extract 24-bit OUI from EUI-48
    encapsulation <<= 16;                 // Shift 24-bit OUI
    encapsulation |= 0x000000FFFE000000;  // Adding 0xFFFE between OUI and
                                          // Extension Identifier
    encapsulation |=
        (EUI_value & 0xFFFFFF);  // Copy 24-bit Extension Identifier from EUI-48

    return encapsulation;
  }

  return EUI_value;
}

/**
 * @brief write data without processing page boundaries. It's up to you not to
 * cross pages. You must call writeEnable() and check that no write is in
 * progress before calling this function.
 *
 * @param address address to start
 * @param src data source
 * @param len len of data to write
 */
void EEPROM_25AA02EXX::simple_write(const uint8_t address, uint8_t *src,
                                    const size_t len) {
  assert(len > 0);
  assert((address + len) <= EEPROM_25AA02EXX_MEMORY_SIZE);

  const uint8_t prefix[2] = {EEPROM_25AA02EXX_WRITE, address};
  spi_dev->write(src, len, prefix, 2);
}

bool EEPROM_25AA02EXX::is_write_in_progress(void) {
  if (_writeBegin)
    return true;
  else
    return (read_status() & EEPROM_25AA02EXX_WIP) ? true : false;
}

/**
 * @brief Prepare internal variables for writing operation
 *
 * @param address Start address
 * @param src Data source
 * @param len Data len
 */
void EEPROM_25AA02EXX::_write_prepare(const uint8_t address, uint8_t *src,
                                      const size_t len) {
  _wo.currentPage = _wo.pageStart = address / EEPROM_25AA02EXX_PAGE_SIZE;
  _wo.pageEnd = (address + len - 1) / EEPROM_25AA02EXX_PAGE_SIZE;
  _wo.totalPages = _wo.pageEnd - _wo.pageStart + 1;

  _wo.src = src;
  _wo.currentAddress = address;
  _wo.lenToWrite = len;

  _wo.lastWIPCheck = millis();
}

void EEPROM_25AA02EXX::_next_write() {
  uint16_t writableLen =
      ((_wo.currentPage + 1) * EEPROM_25AA02EXX_PAGE_SIZE) - _wo.currentAddress;

  const uint8_t prefix[2] = {EEPROM_25AA02EXX_WRITE, _wo.currentAddress};

  if (writableLen < _wo.lenToWrite) {
    spi_dev->write(_wo.src, writableLen, prefix, 2);

    _wo.currentAddress += writableLen;
    _wo.src += writableLen;
    _wo.lenToWrite -= writableLen;
    _wo.currentPage += 1;

  } else {
    spi_dev->write(_wo.src, _wo.lenToWrite, prefix, 2);

    _wo.currentPage += 1;
  }
}

/**
 * @brief write data and take cares of page boundaries for you.
 * This is a blocking function and can take up to 80ms to complete a full 16
 * pages write.
 *
 * @param address address to start
 * @param src data source
 * @param len len of data to write
 */
void EEPROM_25AA02EXX::write(const uint8_t address, uint8_t *src,
                             const size_t len) {
  assert(len > 0);
  assert((address + len) <= EEPROM_25AA02EXX_MEMORY_SIZE);

  // cannot write if an asynchronous process is running
  if (_writeBegin) return;

  _write_prepare(address, src, len);

  // Write each page
  while ((_wo.pageEnd - _wo.currentPage + 1) > 0) {
    write_enable();
    _next_write();

    // Write cycle takes approx. 5ms
    while (read_status() & EEPROM_25AA02EXX_WIP) {
      delay(1);
    }
  }
}

/**
 * @brief
 *
 * @param address
 * @param src
 * @param len
 */
void EEPROM_25AA02EXX::begin_write(const uint8_t address, uint8_t *src,
                                   const size_t len) {
  assert(_writeBuffer != nullptr);
  assert(_writeBufferSize >= len);

  // cannot write if an asynchronous process is still running
  if (_writeBegin) return;

  memcpy(_writeBuffer, src, len);

  _write_prepare(address, _writeBuffer, len);

  _writeBegin = true;
}

/**
 * @brief
 *
 */
void EEPROM_25AA02EXX::process(void) {
  if (!_writeBegin) return;

  if ((_wo.pageEnd - _wo.currentPage + 1) <= 0) {
    _writeBegin = false;
    return;
  }

  if ((unsigned long)(millis() - _wo.lastWIPCheck) < 1) return;

  _wo.lastWIPCheck = millis();

  if (read_status() & EEPROM_25AA02EXX_WIP) return;

  write_enable();
  _next_write();
}