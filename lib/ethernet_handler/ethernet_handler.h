#ifndef ETHERNET_HANDLER_H
#define ETHERNET_HANDLER_H

#include <SPI.h>
#include <Ethernet.h>

#include "debug.h"
#include "misc.h"

class EthernetHandler
{
public:
    EthernetHandler(bool useDHCP = true, int serverPort = 23, int maxClients = 4);
    void initialise();
    void tick();

private:
    bool _use_dhcp;
    int _max_clients;
    int _server_port;
    EthernetServer *_server;
    EthernetClient *_clients;
    bool _link_up = 0;
    uint8_t *_mac_address;

    void _get_ip_address();
    void _initialise_io();
    void _reset_phy();
    void _load_mac_address();
    void _update_link_state();
};

#endif // ETHERNET_HANDLER_H