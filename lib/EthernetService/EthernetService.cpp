#include "EthernetService.h"

EthernetService::EthernetService() : Service(ServiceId::ETHERNET, EthernetServiceConfig())
{
}

void EthernetService::initialise(ServiceContext *ctx)
{
    _initialise(ctx);
    _phy = new SpiEthernetPhy(gpio_w5500_miso, gpio_w5500_mosi, gpio_w5500_sclk, gpio_w5500_csn, gpio_w5500_rstn);
    const char *spiName = _phy->get_spi_instance_name();

    _debugf("Ethernet PHY controller configured as %s", spiName);

    if (_phy->w5500_present())
    {
        _debug("W5500 detected");
    }
    else
    {
        _error("Failed to detect W5500 Ethernet PHY");
    }

    _udp = new EthernetUDP();
    _ntpClient = new NTPClient(*_udp, _config.data.ntpServerAddress, 0, _config.data.ntpUpdateIntervalSeconds * 1000);
}

void EthernetService::_get_ip_address()
{
    if (_config.data.useDhcp)
    {
        _info("Attempting to get IP address over DHCP");
        // This returns 1 on successful connection, 0 if connection failed
        while (Ethernet.begin(_config.data.macAddress) != 1)
        {
            delay(500);
            _info("DHCP request failed");
        }

        _infof("Got IP throught DHCP of %d.%d.%d.%d", Ethernet.localIP()[0], Ethernet.localIP()[1], Ethernet.localIP()[2], Ethernet.localIP()[3]);
    }
    else
    {
        Ethernet.begin(_config.data.macAddress, _config.data.staticIpAddress, _config.data.subnetMask);
        _infof("Using static IP address: %d.%d.%d.%d with subnet mask of %d.%d.%d.%d", _config.data.staticIpAddress[0], _config.data.staticIpAddress[1], _config.data.staticIpAddress[2], _config.data.staticIpAddress[3], _config.data.subnetMask[0], _config.data.subnetMask[1], _config.data.subnetMask[2], _config.data.subnetMask[3]);
    }
}

void EthernetService::_update_link_state()
{
    if (!_linkUp)
    {
        if (Ethernet.linkStatus() == LinkON)
        {
            _info("Ethernet link up");
            _get_ip_address();
            _linkUp = true;
        }
    }
    else
    {
        if (Ethernet.linkStatus() == LinkOFF)
        {
            _linkUp = false;
            _info("Ethernet link down");
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

    uint8_t defaultStaticIpAddress[4] = {192, 168, 1, 1};
    uint8_t defaultSubnetMask[4] = {255, 255, 255, 0};

    memcpy(_config.data.staticIpAddress, defaultStaticIpAddress, sizeof(defaultStaticIpAddress));
    // Could do with memset but less flexible
    memcpy(_config.data.subnetMask, defaultSubnetMask, sizeof(defaultSubnetMask));

    _config.data.serverPort = 80;
    _config.data.maxClients = 4;
    _ctx->eepromHelper->get_mac_address(_config.data.macAddress, sizeof(_config.data.macAddress));

    // NTP
    _config.data.useNtp = true;
    String ntpServer = "europe.pool.ntp.org";
    snprintf(_config.data.ntpServerAddress, ntpServer.length() + 1, "%s\0", ntpServer.c_str());
    _config.data.ntpUpdateIntervalSeconds = 86400;
}

void EthernetService::_print_config()
{
    _info("Config:");
    _infof("\tMAC Address: %02X:%02X:%02X:%02X:%02X:%02X", _config.data.macAddress[0], _config.data.macAddress[1], _config.data.macAddress[2], _config.data.macAddress[3], _config.data.macAddress[4], _config.data.macAddress[5]);
    _infof("\tUse DHCP: %s", _config.data.useDhcp ? "Yes" : "No");
    _infof("\tStatic IP: %d.%d.%d.%d", _config.data.staticIpAddress[0], _config.data.staticIpAddress[1], _config.data.staticIpAddress[2], _config.data.staticIpAddress[3]);
    _infof("\tSubnet Mask: %d.%d.%d.%d", _config.data.subnetMask[0], _config.data.subnetMask[1], _config.data.subnetMask[2], _config.data.subnetMask[3]);
    _infof("\tServer Port: %d", _config.data.serverPort);
    _infof("\tMax clients: %d", _config.data.maxClients);
    _infof("\tUse NTP: %s", _config.data.useNtp ? "Yes" : "No");
    _infof("\tNTP Server: %s", _config.data.ntpServerAddress);
    _infof("\tNTP Update Interval (seconds): %lu", _config.data.ntpUpdateIntervalSeconds);
}

void EthernetService::tick()
{
    _update_link_state();
    if (_linkUp && _ntpClient != NULL)
    {
        // Update returns true if a successful update was made
        // sadly, it returns false if either the update failed
        // or the update interval hadn't passed. So I don't think there's
        // a great way to distinguish between the two, short of tracking ourselves
        if (_ntpClient->update())
        {
            // Standard NTP library doesn't include date
            // Taking code from issue #36 (https://github.com/arduino-libraries/NTPClient/issues/36)
            time_t rawtime = _ntpClient->getEpochTime();
            struct tm *ti;
            ti = localtime(&rawtime);

            int year = ti->tm_year + 1900;
            int month = ti->tm_mon + 1; // tm_mon starts from 0
            int day = ti->tm_mday;

            int hour = _ntpClient->getHours();
            int minute = _ntpClient->getMinutes();
            int second = _ntpClient->getSeconds();

            _info("!!! NTP Sync - logging time is about to jump (maybe backwards) !!!");
            _ctx->timeHelper->set_time(year, month, day, hour, minute, second);
            _info("New time set");
        }
    }
}
