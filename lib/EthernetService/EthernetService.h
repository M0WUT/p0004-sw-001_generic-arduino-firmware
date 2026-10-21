#ifndef ETHERNETSERVICE_H
#define ETHERNETSERVICE_H

#include "pinout.h"
#include "Service.h"
#include "ServiceContext.h"
#include "SpiEthernetPhy.h"

class EthernetService : public Service<EthernetServiceConfig>
{
public:
    EthernetService(); // uint8_t mac[6], bool useDHCP = true, int serverPort = 23, int maxClients = 4);
    void tick();

private:
    SpiEthernetPhy _phy;
    EthernetServer *_server;
    EthernetClient *_clients;
    bool _link_up = 0;

    void _get_ip_address();
    void _update_link_state();
    ConfigData *_getConfig() { return &_config; }
};

#endif // ETHERNETSERVICE_H
