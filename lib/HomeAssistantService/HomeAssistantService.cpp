#include "HomeAssistantService.h"

HomeAssistantService::HomeAssistantService() : Service(ServiceId::HOME_ASSISTANT, HomeAssistantServiceConfig()), _client(EthernetClient()), _haDevice(HADevice()), _haMqtt(HAMqtt(_client, _haDevice))
{
}

void HomeAssistantService::initialise(ServiceContext *ctx)
{
    _initialise(ctx);
    _haDevice.setName(_config.data.haDeviceName);
    _haDevice.setManufacturer("M0WUT");
    _haDevice.setModel("Generic");
    _haDevice.setSoftwareVersion(VERSION_STRING);
    _haDevice.setUniqueId((const byte *)_config.data.haDeviceName, sizeof(_config.data.haDeviceName));
}

void HomeAssistantService::tick()
{
    if (!_config.data.enabled)
    {
        return;
    }
    EthernetService *service = (EthernetService *)_ctx->serviceHelper->get_service(ServiceId::ETHERNET);
    bool linkUp = service->link_up();
    if (!_mqttRunning)
    {
        if (linkUp)
        {
            const char *username = *_config.data.mqttUsername ? _config.data.mqttUsername : nullptr;
            const char *password = *_config.data.mqttPassword ? _config.data.mqttPassword : nullptr;

            if (_haMqtt.begin(_config.data.mqttBrokerAddress, username, password))
            {
                _mqttRunning = true;
                _info("Successfully connected to Home Assistant MQTT broker");
            }
            else
            {
                _warning("Connection to Home Assistant MQTT broker failed");
            }
        }
    }
    else
    {
        if (linkUp)
        {
            // Loop apparently handles reconnect
            _haMqtt.loop();
            if (!_haMqtt.isConnected())
            {
                _warning("Home Assistant MQTT broker disconnected");
                _mqttRunning = false;
            }
        }
    }
}

void HomeAssistantService::get_config_fields(ConfigFieldList &fields) const
{
    fields.addBoolean("haEnabled", "Home Assistant Connection enabled?", _config.data.enabled);
    fields.addIpLike("mqttBrokerAddress", "MQTT Broker IP Address", _config.data.mqttBrokerAddress);
    fields.addString("mqttUsername", "MQTT Broker Username (optional)", _config.data.mqttUsername);
    fields.addString("mqttPasword", "MQTT Broker Password (optional)", _config.data.mqttPassword);
}

bool HomeAssistantService::validate_config_field(const char *id, const ConfigValue &value, char *error, size_t errorSize)
{
    return true;
}

bool HomeAssistantService::update_config_field(const char *id, const ConfigValue &value, char *error, size_t errorSize)
{
    return true;
}

void HomeAssistantService::get_status_fields(StatusFieldList &fields) const
{
    fields.add("Connected to broker?", _haMqtt.isConnected() ? "Yes" : "No");
}

void HomeAssistantService::_generate_default_config()
{
#ifdef M0WUT_HA_SETTINGS
#include "m0wut_ha_login.h"
    _config.data.enabled = true;
    for (int i = 0; i < 4; i++)
    {
        _config.data.mqttBrokerAddress[i] = M0WUT_BROKER_IP[i];
    }
    snprintf(_config.data.mqttUsername, sizeof(_config.data.mqttUsername), "%s", M0WUT_BROKER_USERNAME);
    snprintf(_config.data.mqttPassword, sizeof(_config.data.mqttPassword), "%s", M0WUT_BROKER_PASSWORD);

#else
    _config.data.enabled = false;
    for (int i = 0; i < 4; i++)
    {
        _config.data.mqttBrokerAddress[i] = 0;
    }
    // Empty string
    _config.data.mqttUsername[0] = '\0';
    _config.data.mqttPassword[0] = '\0';
#endif
    uint8_t macAddress[6];
    _ctx->eepromHelper->get_mac_address(macAddress, sizeof(macAddress));

    snprintf(_config.data.haDeviceName, sizeof(_config.data.haDeviceName), "GTE%02X%02X%02X", macAddress[3], macAddress[4], macAddress[5]);
}
