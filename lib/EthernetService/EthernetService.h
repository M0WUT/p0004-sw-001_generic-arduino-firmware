#pragma once

#include "pinout.h"
#include "BaseService.h"
#include "ServiceContext.h"
#include "SpiEthernetPhy.h"
#include <EthernetUdp.h>
#include <NTPClient.h>

struct EthernetServiceConfigData
{
    uint8_t macAddress[6];
    bool useDhcp;
    char hostname[32];
    uint8_t staticIpAddress[4];
    uint8_t subnetMask[4];
    bool useNtp;
    char ntpServerAddress[32];
    uint32_t ntpUpdateIntervalMinutes;
};

class EthernetServiceConfig : public ConfigTemplate<EthernetServiceConfigData>
{
public:
    EthernetServiceConfig() : ConfigTemplate<EthernetServiceConfigData>(1, {}) {}
    size_t size() { return sizeof(EthernetServiceConfigData); }
};

class EthernetService : public Service<EthernetServiceConfig>
{
public:
    // Constructor
    EthernetService();

    // Required overrides from BaseService
    void initialise(ServiceContext *ctx);
    void tick();
    void get_config_fields(ConfigFieldList &fields) const;
    bool validate_config_field(const char *, const ConfigValue &, char *, size_t);
    bool update_config_field(const char *id, const ConfigValue &value,
                             char *error, size_t errorSize);

    // Class specific
    bool link_up();

private:
    // Required overrides from BaseService
    ConfigData *_get_config() { return &_config; }
    const char *get_name() override { return "Ethernet"; }
    void _generate_default_config();

    // Class specific
    SpiEthernetPhy *_phy;
    EthernetServer *_server;
    EthernetClient *_clients;
    bool _linkUp = false;

    void _get_ip_address();
    void _update_link_state();

    // NTP related stuff
    EthernetUDP *_udp = nullptr;
    NTPClient *_ntpClient = nullptr;
};
