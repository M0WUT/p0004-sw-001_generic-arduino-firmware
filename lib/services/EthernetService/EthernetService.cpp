#include "EthernetService.h"

EthernetService::EthernetService() : Service(ServiceID::ETHERNET), _phy(gpio_w5500_miso, gpio_w5500_mosi, gpio_w5500_sclk, gpio_w5500_cs, gpio_w5500_rstn)
{
    // size_t mac_size = 6 * sizeof(byte);
    // _mac_address = (uint8_t *)malloc(mac_size);
    // memcpy(_mac_address, mac, mac_size);
    // _use_dhcp = use_dhcp;
    // _server_port = server_port;
    // _max_clients = max_clients;
}

void EthernetService::initialise()
{
    _server = new EthernetServer(_server_port);
    EthernetClient _clients[_max_clients];
    _link_up = false;
    _debug_printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", _mac_address[0], _mac_address[1], _mac_address[2], _mac_address[3], _mac_address[4], _mac_address[5]);
    // _get_ip_address();
}

void EthernetService::_get_ip_address()
{
    if (_use_dhcp)
    {
        _debug_println("Attempting to get IP address over DHCP");
        // This returns 1 on successful connection, 0 if connection failed
        while (Ethernet.begin(_mac_address) != 1)
        {
            delay(500);
            _debug_println("DHCP request failed");
        }

        _debug_printf("Got IP throught DHCP of %d.%d.%d.%d\n", Ethernet.localIP()[0], Ethernet.localIP()[1], Ethernet.localIP()[2], Ethernet.localIP()[3]);
    }
    else
    {
        // Ethernet.begin(mac_address, ip_address, subnet_mask);
        // DEBUG_PRINT("Initialising with static IP of ");
        // DEBUG_PRINTLN(ip_address);
        ;
    }

    _debug_println("Starting server...");
    _server->begin();
    delay(1000);
    _debug_println("Server started.");
}

void EthernetService::_update_link_state()
{
    if (!_link_up)
    {
        if (Ethernet.linkStatus() == LinkON)
        {
            _debug_println("Ethernet connected");
            _get_ip_address();
            _link_up = 1;
        }
    }
    else
    {
        if (Ethernet.linkStatus() == LinkOFF)
        {
            _link_up = 0;
            _debug_println("Ethernet disconnected");
        }
        else
        {
            Ethernet.maintain(); // Keeps DHCP leases active
        }
    }
}

void EthernetService::tick()
{
    _update_link_state();
}
