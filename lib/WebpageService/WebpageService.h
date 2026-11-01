#pragma once

#include "pinout.h"
#include "BaseService.h"
#include "ServiceContext.h"
#include "EthernetServer.h"
#include "EthernetClient.h"
#include "ServiceId.h"
#include "EthernetService.h"
#include "ServiceHelper.h"

constexpr size_t HTTP_LINE_SIZE = 192;
constexpr size_t HTTP_BODY_SIZE = 256;
constexpr size_t MESSAGE_SIZE = 128;

class ServiceHelper;

struct WebpageServiceConfigData
{
    uint16_t serverPort;
    uint8_t maxClients;
};

class WebpageServiceConfig : public ConfigTemplate<WebpageServiceConfigData>
{
public:
    WebpageServiceConfig() : ConfigTemplate<WebpageServiceConfigData>(1, {}) {}
    size_t size() { return sizeof(WebpageServiceConfigData); }
};

class WebpageService : public Service<WebpageServiceConfig>
{
public:
    WebpageService();
    void initialise(ServiceContext *ctx);
    void tick();
    void get_config_fields(ConfigFieldList &fields) const;
    bool validate_config_field(const char *, const ConfigValue &, char *, size_t);
    bool update_config_field(const char *id, const ConfigValue &value,
                             char *error, size_t errorSize);

    void generate_config_page(Print &out);

private:
    ConfigData *_get_config() { return &_config; };
    void _generate_default_config();
    void _print_config();
    EthernetServer *_server;
    bool _serverRunning = false;
    void _html_escape(Print &out, const char *outputStr);
    void _handle_client(EthernetClient &client);
    static void _send_response(EthernetClient &client, int code, const char *reason, const char *content);
    static bool _read_line(EthernetClient &client, char *buf, size_t capacity);
    bool _url_decode(const char *src, size_t length, char *dst, size_t capacity);
    int _hex_digit(char c);
    bool _parse_unsigned(const char *p, size_t &result);
    bool parseUpdateBody(const char *body, size_t &serviceIndex, char *id, size_t idSize,
                         char *value, size_t valueSize);
};