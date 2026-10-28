#ifndef ETHERNETSERVICE_H
#define ETHERNETSERVICE_H

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
    uint8_t staticIpAddress[4];
    uint8_t subnetMask[4];
    uint16_t serverPort;
    uint8_t maxClients;
    bool useNtp;
    char ntpServerAddress[32];
    uint32_t ntpUpdateIntervalSeconds;
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
    EthernetService();
    void initialise(ServiceContext *ctx);
    void tick();

private:
    SpiEthernetPhy _phy;
    EthernetServer *_server;
    EthernetClient *_clients;
    bool _linkUp = false;

    void _get_ip_address();
    void _update_link_state();
    ConfigData *_getConfig() { return &_config; }
    void _generate_default_config();
    void _print_config();

    // NTP related stuff
    EthernetUDP *_udp = NULL;
    NTPClient *_ntpClient = NULL;
};

#endif // ETHERNETSERVICE_H
