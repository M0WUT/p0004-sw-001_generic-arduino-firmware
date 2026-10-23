#ifndef ETHERNETSERVICE_H
#define ETHERNETSERVICE_H

#include "pinout.h"
#include "Service.h"
#include "ServiceContext.h"
#include "SpiEthernetPhy.h"

class EthernetService : public Service<EthernetServiceConfig>
{
public:
    EthernetService();
    void tick();

private:
    SpiEthernetPhy _phy;
    EthernetServer *_server;
    EthernetClient *_clients;
    bool _link_up = 0;

    void _get_ip_address();
    void _update_link_state();
    ConfigData *_getConfig() { return &_config; }
    void _generate_default_config();
    void _print_config();
};

#endif // ETHERNETSERVICE_H
