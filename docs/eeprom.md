# EEPROM format 
GTE software consists of a number of running "services" that perform the operations of the device. It also consists of a number of "helpers" that perform functionality required by all services e.g. access to the EEPROM / debug UART / error handling etc.

## Overview
Each service may have specific data that is required to be stored in non-volatile memory - referred to as an 'entry'. As such, each service handles its section of EEPROM independently. This allows for behaviour of different services to be changed without affecting others. Services identify themselves using their `ServiceId` (located in `lib\ServiceId\ServiceId.h`). Each time a new service is created, this enum must be updated. Once a `ServiceId` has been issued, it must not be re-used to prevent conflicts. Each service must define a struct that contains the data it wishes to save. The `EepromHelper` will issue a base address (based on the file allocation table stored at the start of the EEPROM) and the struct can be saved by writing `sizeof(<struct>)` bytes to `<base-address-issued-by-EepromHelper>`. Conversly, the struct can be populated by doing a read of `sizeof(<struct>)` bytes back to a pointer in memory to an struct initialised with default values. Note that this means that storage of pointers is not really practical and any array-like members should be initialised as fixed size to allocate the correct amount of memory. e.g. `char *myString` is invalid - we need enough space to store the actual string so this should be `char myString[16]`. N.B. don't forget room for the null-terminator. `EepromHelper` will not perform sanity checking of the data - that should take place when the data was saved to the struct in memory. This allows for the following read/write code 
```
    struct test_t
    {
        uint8_t x;
        uint16_t y;
    } test, test2;

    // Populate with dummy values
    test.x = 0x12;
    test.y = 0x3456;

    // Write contents of "test" to address 0
    write_bytes(0, (uint8_t *)&test, sizeof(test));

    // Read back into "test2" struct
    read_bytes(0, (uint8_t *)&test2, sizeof(test2));
```

## Versioning
Every individual config in the EEPROM must start with a byte containing the version number of the struct used. The next 2 bytes should be the 16-bit length of the config (including these 3 header bytes and the checksum byte at the end).

Then fill in the user data.

The final byte of an config should be a CRC8 checksum over all data in the config (apart from the checksum itself). The checksum shall be CRC8 with a polynomial of 0x07 (*x⁸ + x² + x + 1*), an initial value of 0 and an XOR output of 0. This allows use of [Arduino CRC library](https://github.com/RobTillaart/CRC) with default settings.

## Endian-ness
Bytes are stored little-endian.


## Config allocation table (CAT)
The start of the EEPROM primarily contains the base addresses for each config. To distinguish an intialised EEPROM from a blank (or one used for a previous purpose, but this is unlikely), the first 3 byte (address `0x00` - `0x02`) should be set to ASCII "LID" (`0x4C, 0x49, 0x44` respectively). All other values should be treated as the EEPROM contains invalid data and should be initialised to default values. Should the format of the CAT require updating in the future, a different series of initialisation bytes shall be used.

The next byte contains how many entries exist in the CAT. Each config is a two-byte base address (note little-endian) of the config. All ServiceId from `0` - `(<number-of-entries> - 1)` must be enumerated. If a ServiceId doesn't have an config in the EEPROM, its base address must be set to `0xFFFF`. e.g. if the CAT has details for 3 entries, ServiceId 0 has a base address of 0x1234, Service ID 1 does not have an config, Service ID 2 has a base address of 0x5678, then the start of the EEPROM should look like the following: N.B. The address of the CRC is not absolute, it should be the next byte after the CAT finishes (i.e. at address 4 + 2*<value in `CAT_LENGTH`>)

| Absolute Address |  Name | Value | Description | Include in length calculation? |Include in CRC calculation? |
|---|---|---|---|---|---|
| `0x00` | `INIT[0]` | `0x4C` | First initialisation byte ("L") | Y | Y |
| `0x01` | `INIT[1]` | `0x49` | Second initialisation byte ("I") | Y | Y |
| `0x02` | `INIT[2]` | `0x44` | Third initialisation byte ("D") | Y | Y |
| `0x03` | `CAT_LENGTH` | `0x03` | CAT length (number of entries, not bytes) | Y | Y |
| `0x04` | `CAT[0]_LSB` | `0x34` | LSB of Base address of config for service 0 (0x1234) | Y | Y |
| `0x05` | `CAT[0]_MSB` | `0x12` | MSB of Base address of config for service 0 (0x1234) | Y | Y |
| `0x06` | `CAT[1]_LSB` | `0xFF` | LSB of Base address of config for service 1 (Not assigned) | Y | Y |
| `0x07` | `CAT[1]_MSB` | `0xFF` | MSB of Base address of config for service 1 (Not assigned) | Y | Y |
| `0x08` | `CAT[2]_LSB` | `0x78` | LSB of Base address of config for service 2 (0x5678) | Y | Y |
| `0x09` | `CAT[2]_MSB` | `0x56` | MSB of Base address of config for service 2 (0x5678) | Y | Y |
| `0x0A` | `CAT_CRC` | `0x1F` | CRC8 of CAT | Y | N |

## Config
Note addresses are given as offsets to the config base address as that can vary.
CONFIG_LENGTH is 16 bits as a single entry may exceed 255 bytes. It should include bytes used for CONFIUG_VERSION, CONFIG_LENGTH and CONFIG_CRC i.e. be 4 larger than the size of the config itself.

In the following example, the config data is assumed to be N bytes long.

| Relative Address | Name | Value | Description | Include in length calculation? |Include in CRC calculation? |
|---|---|---|---|---|---|
| `0x00` | `CONFIG_VERSION` | `1` | Version of config struct saved. Any change in struture requires the version to be incremented | Y | Y |
| `0x01` | `CONFIG_LENGTH[0]` | `LSB of <N+4>` | Length of config (LSB) | Y | Y |
| `0x02` | `CONFIG_LENGTH[1]` | `MSB of <N+4>` | Length of config (MSB) | Y | Y |
| `0x03` | `CONFIG_DATA[0]` | `?` | First byte of config data | Y | Y |
| ... | ... | ... | ... | Y |  Y |
| `N+2` |  `CONFIG_DATA[N-1]` | `?` | Last byte of config data | Y | Y |
| `N+3` | `CONFIG_CRC` | ? | CRC8 | Y | N |













