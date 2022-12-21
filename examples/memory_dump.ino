/**
 * @brief Memory dump example for library eeprom_25AA02EXX
 *
 */
#include <Arduino.h>
#include <eeprom_25AA02EXX.h>

// EEPROM SPI Chip Select
#define EEPROM_CS 7

uint8_t pages[EEPROM_25AA02EXX_TOTAL_PAGES];
EEPROM_25AA02EXX eeprom = EEPROM_25AA02EXX();

template <typename IntType> void printHex(IntType val, Print *ptr = &Serial) {
  for (int8_t shift = 8 * sizeof(val) - 4; shift >= 0; shift -= 4) {
    uint8_t hexDigit = (val >> shift) & 0xF;
    ptr->print(hexDigit, HEX);
    if (((shift & 0xF) == 0) && (shift > 0)) {
      ptr->print(" ");
    }
  }
}

/**
 * @brief Dump the entire memory array
 *
 */
static void dumpMemory(void) {
  Serial.println("Begin of memory dump");

  for (uint8_t i = 0; i <= EEPROM_25AA02EXX_TOTAL_PAGES; i++) {
    Serial.print("Dump page ");
    printHex(i);
    Serial.print("h - ");

    eeprom.read(i * EEPROM_25AA02EXX_PAGE_SIZE, pages,
                EEPROM_25AA02EXX_PAGE_SIZE);

    for (uint8_t j = 0; j < EEPROM_25AA02EXX_PAGE_SIZE; j++) {
      printHex(pages[j]);
      Serial.print(" ");
    }
    Serial.println();
  }

  Serial.println("End of memory dump");
}

/**
 * @brief Dump the Unique Identification Number
 *
 */
static void dumpEUI(void) {
  Serial.println("Reading EUI");

  Serial.print("EUI 48 : ");
  printHex(eeprom.read_EUI(EEPROM_25AA02EXX_EUI_Format::EUI48));
  Serial.println();

  Serial.print("EUI 64 : ");
  printHex(eeprom.read_EUI(EEPROM_25AA02EXX_EUI_Format::EUI64));
  Serial.println();

  Serial.print("EUI 64 FROM EUI 48 : ");
  printHex(eeprom.read_EUI(EEPROM_25AA02EXX_EUI_Format::EUI64_FROM_EUI48));
  Serial.println();
}

void setup() {
  Serial.begin(9600);
  eeprom.begin_SPI(EEPROM_CS);
}

void loop() {
  dumpMemory();
  dumpEUI();

  delay(5000);
}
