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
Every individual struct in the EEPROM must start with a byte containing the version number of the struct used. The next 2 bytes should be the length of the entry (including these 3 header bytes and the checksum byte at the end).

Then fill in the user data.

The final byte of an entry should be a CRC8 checksum over all data in the entry (apart from the checksum itself). The checksum shall be CRC8 with a polynomial of 0x07 (*x⁸ + x² + x + 1*), an initial value of 0 and an XOR output of 0. This allows use of [Arduino CRC library](https://github.com/RobTillaart/CRC) with default settings.

## Endian-ness
Bytes are stored little-endian.


## Entry allocation table (EAT)
The start of the EEPROM primarily contains the base addresses for each entry. To distinguish an intialised EEPROM from a blank (or one used for a previous purpose, but this is unlikely), the first 3 byte (address `0x00` - `0x02`) should be set to ASCII "LID" (`0x4C, 0x49, 0x44` respectively). All other values should be treated as the EEPROM contains invalid data and should be initialised to default values.

The next byte contains how many entries exist in the EAT. Each entry is a two-byte base address (note little-endian) of the entry. All ServiceId from `0` - `(<number-of-entries> - 1)` must be enumerated. If a ServiceId doesn't have an entry in the EEPROM, its base address must be set to `0xFFFF`. e.g. if the EAT has details for 3 entries, ServiceId 0 has a base address of 0x1234, Service ID 1 does not have an entry, Service ID 2 has a base address of 0x5678, then the start of the EEPROM should look like the following: N.B. The address of the CRC is not absolute, it should be the next byte after the EAT finishes (i.e. at address 4 + 2*<value in `EAT_LENGTH`>)

| Address | Register | Name | Value | Description |
|---:|---:|---|---:|---|
| `0x00` | 0 | `INIT0` | `0x4C` | First initialisation byte ("L") |
| `0x01` | 1 | `INIT1` | `0x49` | Second initialisation byte ("I") |
| `0x02` | 2 | `INIT2` | `0x44` | Third initialisation byte ("D") |
| `0x03` | 3 | `EAT_LENGTH` | `0x03` | EAT length (number of entries, not bytes) |
| `0x04` | 4 | `EAT[0]_LSB` | `0x34` | First EAT entry (0x1234) LSB |
| `0x05` | 5 | `EAT[0]_MSB` | `0x12` | First EAT entry (0x1234) MSB |
| `0x06` | 6 | `EAT[1]_LSB` | `0xFF` | Second EAT entry (Not assigned) LSB |
| `0x07` | 7 | `EAT[1]_MSB` | `0xFF` | Second EAT entry (Not assigned) MSB |
| `0x08` | 8 | `EAT[2]_LSB` | `0x78` | Third EAT entry (0x5678) LSB |
| `0x09` | 9 | `EAT[2]_MSB` | `0x56` | Third EAT entry (0x5678) MSB |
| `0x0A` | 10 | `EAT_CRC` | `0x1F` | CRC8 of EAT |








