#ifndef ETHERNETSERVICE_H
#define ETHERNETSERVICE_H

#include "pinout.h"
#include "services/Service/Service.h"
#include "drivers/SpiEthernetPhy/SpiEthernetPhy.h"

class EthernetService : public Service
{
public:
    EthernetService(); // uint8_t mac[6], bool useDHCP = true, int serverPort = 23, int maxClients = 4);
    void initialise();
    void tick();

private:
    SpiEthernetPhy _phy;
    bool _use_dhcp;
    int _max_clients;
    int _server_port;
    EthernetServer *_server;
    EthernetClient *_clients;
    bool _link_up = 0;
    uint8_t *_mac_address;

    void _get_ip_address();
    void _update_link_state();
};

#endif // ETHERNETSERVICE_H
