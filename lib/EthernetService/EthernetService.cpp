#include "EthernetService.h"

EthernetService::EthernetService() : Service(ServiceId::ETHERNET, EthernetServiceConfig()), _phy(gpio_w5500_miso, gpio_w5500_mosi, gpio_w5500_sclk, gpio_w5500_cs, gpio_w5500_rstn)
{
}

void EthernetService::_get_ip_address()
{
    // if (_use_dhcp)
    // {
    //     _info("Attempting to get IP address over DHCP");
    //     // This returns 1 on successful connection, 0 if connection failed
    //     while (Ethernet.begin(_mac_address) != 1)
    //     {
    //         delay(500);
    //         _info("DHCP request failed");
    //     }

    //     _infof("Got IP throught DHCP of %d.%d.%d.%d\r\n", Ethernet.localIP()[0], Ethernet.localIP()[1], Ethernet.localIP()[2], Ethernet.localIP()[3]);
    // }
    // else
    // {
    //     // Ethernet.begin(mac_address, ip_address, subnet_mask);
    //     // DEBUG_PRINT("Initialising with static IP of ");
    //     // DEBUG_PRINTLN(ip_address);
    //     ;
    // }

    // _info("Starting server...");
    // _server->begin();
    // delay(1000);
    // _info("Server started.");
}

void EthernetService::_update_link_state()
{
    if (!_link_up)
    {
        if (Ethernet.linkStatus() == LinkON)
        {
            _info("Ethernet connected");
            _get_ip_address();
            _link_up = 1;
        }
    }
    else
    {
        if (Ethernet.linkStatus() == LinkOFF)
        {
            _link_up = 0;
            _info("Ethernet disconnected");
        }
        else
        {
            Ethernet.maintain(); // Keeps DHCP leases active
        }
    }
}

void EthernetService::_generate_default_config()
{
    _config.data.useDhcp = true;
    for (int i = 0; i < 4; i++)
    {
        _config.data.staticIpAddress[i] = 0;
    }
    _config.data.serverPort = 80;
    _config.data.maxClients = 4;
}

void EthernetService::tick()
{
    _update_link_state();
}
