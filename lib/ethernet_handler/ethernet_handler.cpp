#include "ethernet_handler.h"

EthernetHandler::EthernetHandler(bool use_dhcp, int server_port, int max_clients)
{
    _use_dhcp = use_dhcp;
    _server_port = server_port;
    _max_clients = max_clients;
}

void EthernetHandler::initialise()
{
    _server = new EthernetServer(_server_port);
    EthernetClient _clients[_max_clients];
    _link_up = false;
    _mac_address = (uint8_t *)malloc(6 * sizeof(uint8_t));
    _load_mac_address();
    DEBUG_PRINTF("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", _mac_address[0], _mac_address[1], _mac_address[2], _mac_address[3], _mac_address[4], _mac_address[5]);
    _initialise_io();
    _reset_phy();
    // _get_ip_address();
}

void EthernetHandler::_get_ip_address()
{
    if (_use_dhcp)
    {
        DEBUG_PRINTLN("Attempting to get IP address over DHCP");
        // This returns 1 on successful connection, 0 if connection failed
        while (Ethernet.begin(_mac_address) != 1)
        {
            delay(500);
            DEBUG_PRINTLN("FAILED");
        }

        DEBUG_PRINTF("Got IP throught DHCP of %d.%d.%d.%d\n", Ethernet.localIP()[0], Ethernet.localIP()[1], Ethernet.localIP()[2], Ethernet.localIP()[3]);
    }
    else
    {
        // Ethernet.begin(mac_address, ip_address, subnet_mask);
        // DEBUG_PRINT("Initialising with static IP of ");
        // DEBUG_PRINTLN(ip_address);
        ;
    }

    DEBUG_PRINTLN("Starting server...");
    _server->begin();
    delay(1000);
    DEBUG_PRINTLN("Server started.");
}

void EthernetHandler::_initialise_io()
{
    SPI.setMISO(gpio_w5500_miso);
    SPI.setMOSI(gpio_w5500_mosi);
    SPI.setSCLK(gpio_w5500_sck);
    // Although the CS pin is a hardware CS pin
    // assigning to the SPI bus causing extra toggling
    // which breaks communication so tell SPI driver it
    // doesn't have a CS...
    SPI.setSSEL(PNUM_NOT_DEFINED);
    SPI.begin();
    // ... and let the Ethernet driver handle it exclusively
    Ethernet.init(gpio_w5500_cs);

    DEBUG_PRINTLN("Started W5500 on " + spi_controller_name(&SPI));

    pinMode(gpio_w5500_rstn, OUTPUT);
    digitalWrite(gpio_w5500_rstn, HIGH);
    delay(10);
}

void EthernetHandler::_reset_phy()
{
    digitalWrite(gpio_w5500_rstn, LOW);
    delay(10);
    digitalWrite(gpio_w5500_rstn, HIGH);
    delay(10);
}

void EthernetHandler::_load_mac_address()
{
    for (int i = 0; i < 6; i++)
    {
        _mac_address[i] = i + 2; // @TODO
    }
}

void EthernetHandler::_update_link_state()
{
    if (!_link_up)
    {
        if (Ethernet.linkStatus() == LinkON)
        {
            DEBUG_PRINTLN("Ethernet connected");
            _get_ip_address();
            _link_up = 1;
        }
    }
    else
    {
        if (Ethernet.linkStatus() == LinkOFF)
        {
            _link_up = 0;
            DEBUG_PRINTLN("Ethernet disconnected");
        }
        else
        {
            Ethernet.maintain(); // Keeps DHCP leases active
        }
    }
}

void EthernetHandler::tick()
{
    _update_link_state();
}
