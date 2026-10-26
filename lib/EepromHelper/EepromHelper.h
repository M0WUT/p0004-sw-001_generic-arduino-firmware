#ifndef EEPROMHELPER_H
#define EEPROMHELPER_H

#include "eeprom_24aa256uid.h"
#include "ServiceId.h"
#include "HelperContext.h"
#include "BaseHelper.h"
#include <CRC8.h>

constexpr uint16_t UNINITIALISED_CONFIG_BASE_ADDRESS = 0xFFFF;

class EepromHelper : public BaseHelper
{

public:
    EepromHelper(EEPROM24AA256UID eeprom, HelperContext *ctx);
    bool read_service_config(ServiceId serviceId, void *config, size_t configSize, uint8_t version);
    void write_service_config(ServiceId serviceId, void *config, size_t configSize, uint8_t version);
    void hexdump(uint16_t start_addr, uint16_t size);
    uint8_t get_config_version(ServiceId serviceId);
    void get_mac_address(uint8_t *buffer, size_t bufferSize);

private:
    EEPROM24AA256UID _eeprom;
    int read_bytes(uint16_t regAddr, uint8_t *data, int numBytes, CRC8 *crc = NULL);
    int write_bytes(uint16_t regAddr, uint8_t *data, int numBytes, CRC8 *crc = NULL);
    uint8_t read8(uint16_t regAddr);
    void write8(uint16_t regAddr, uint8_t data);
    uint8_t read16(uint16_t regAddr);
    void write16(uint16_t regAddr, uint16_t data);
    bool _eeprom_initialised();
    void initialise_eeprom();
    void _read_cat_table();
    void _print_cat_table();
    CRC8 _get_crc();
    void _write_cat_base_address(ServiceId serviceId, uint16_t baseAddress);
    uint16_t _cat_table_size();
    uint8_t _calculate_cat_crc();
    uint8_t _calculate_expected_cat_crc();
    bool _verify_cat_crc();
    uint16_t _allocate_new_config(ServiceId serviceId, size_t configSize);
    uint16_t _get_config_base_address(ServiceId serviceId);
    uint16_t _get_config_data_base_address(ServiceId serviceId);
    uint16_t _get_config_size(ServiceId serviceId);
    uint8_t _get_config_crc(ServiceId serviceId);
    uint16_t _serviceBaseAddresses[NUM_KNOWN_SERVICES];
    const char _initValues[3] = {'L', 'I', 'D'};
    const int _configDataSizeToConfigSizeOffset = 4;
#ifdef DEBUG_EEPROM
    uint8_t *_fakeEeprom; // Used for debugging to have the EEPROM in RAM, not used normally
#endif
};

#endif // EEPROMHELPER_H
