
#include "WebpageService.h"

WebpageService::WebpageService() : Service(ServiceId::WEBPAGE, WebpageServiceConfig())
{
}

void WebpageService::initialise(ServiceContext *ctx)
{
    _initialise(ctx);
    _server = new EthernetServer(_config.data.serverPort);
}

void WebpageService::tick()
{
    EthernetService *service = (EthernetService *)_ctx->serviceHelper->get_service(ServiceId::ETHERNET);
    bool linkUp = service->link_up();
    if (_serverRunning)
    {
        if (linkUp)
        {
            EthernetClient client = _server->available();
            if (client)
            {
                client.setTimeout(2000);
                _handle_client(client);
                client.flush();
                client.stop();
            }
        }
        else
        {
            // Link has died since last check
            delete _server;
            _serverRunning = false;
        }
    }
    else if (linkUp)
    {
        // Link has been established since last check - start server
        _server = new EthernetServer(_config.data.serverPort);
        _server->begin();
        _serverRunning = true;
    }
}

void WebpageService::get_config_fields(ConfigFieldList &fields) const
{
    fields.addInteger("serverPort", "Server Port", _config.data.serverPort, 0, 65535);
    fields.addInteger("maxClients", "Maximum number of clients", _config.data.maxClients, 1, 10);
}

bool WebpageService::validate_config_field(const char *, const ConfigValue &, char *, size_t)
{
    return true;
}

bool WebpageService::update_config_field(const char *id, const ConfigValue &value, char *error, size_t errorSize)
{
    return true;
}

void WebpageService::generate_config_page(Print &out)
{
    BaseService **services = _ctx->serviceHelper->get_service_list();
    int numServices = _ctx->serviceHelper->get_num_services();
    // HTML header and CSS - chunk 1
    out.print(F(R"HTML(<!DOCTYPE html><html lang="en"><head><meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1"><title>STM32 Configuration</title>
<style>
:root{--bg:#666666;--accent:#e1970c;--fg:#fff}
body{background:var(--bg);color:var(--fg);font-family:Arial,sans-serif;margin:20px}
.tabs{display:flex;flex-wrap:wrap;gap:5px;border-bottom:2px solid var(--accent)}
.tab{padding:12px 20px;background:#555;color:#fff;border:1px solid #888;cursor:pointer}
.tab.active{background:var(--accent);border-color:var(--accent);font-weight:bold}
)HTML"));

    // CSS - chunk 2
    out.print(F(R"HTML(
.panel{display:none;border:1px solid #888;padding:15px;overflow-x:auto}.panel.active{display:block}
table{width:100%;min-width:600px;border-collapse:collapse}
th,td{text-align:left;padding:12px;border-bottom:1px solid #888;vertical-align:middle}
th{background:#555;font-size:13px}.parameter{width:30%;font-weight:600}
.current{width:22%;font-size:2.2rem;font-weight:bold;color:var(--accent);white-space:nowrap}
.new-value{width:30%}.action{width:18%}
.limits{display:block;margin-top:5px;font-size:12px;font-weight:normal;color:#eee}
)HTML"));

    // CSS - chunk 3
    out.print(F(R"HTML(
input[type=text],input[type=number]{width:100%;max-width:210px;padding:9px;font-size:16px;box-sizing:border-box;background:#fff;color:#333;border:2px solid #999;border-radius:4px}
input:focus{outline:2px solid var(--accent)}
input[type=checkbox]{width:22px;height:22px;accent-color:var(--accent)}
input[type=number]::-webkit-inner-spin-button,input[type=number]::-webkit-outer-spin-button{-webkit-appearance:none;margin:0}
input[type=number]{appearance:textfield;-moz-appearance:textfield}
.update{padding:10px 18px;background:var(--accent);color:white;border:none;border-radius:4px;cursor:pointer;font-weight:bold}
.update:disabled{opacity:.5;cursor:wait}.status{display:block;margin-top:6px;font-size:12px;overflow-wrap:anywhere}
.ok{color:#fff}.error{color:#ffcccc}
</style>
)HTML"));

    // JavaScript - chunk 1
    out.print(F(R"HTML(
<script>
const successTimers = new WeakMap();
function showTab(index){
 document.querySelectorAll('.tab').forEach(t=>t.classList.toggle('active',Number(t.dataset.service)===index));
 document.querySelectorAll('.panel').forEach(p=>p.classList.toggle('active',p.id==='panel'+index));
}
)HTML"));

    // JavaScript - chunk 2
    out.print(F(R"HTML(
function clearSuccessTimer(row){
 const t=successTimers.get(row);
 if(t!==undefined){clearTimeout(t);successTimers.delete(row)}
}
async function updateField(button){
 const row=button.closest('tr'),input=row.querySelector('input'),current=row.querySelector('.current'),status=row.querySelector('.status');
 clearSuccessTimer(row);
 if(!input.checkValidity()){input.reportValidity();status.textContent='Invalid input';status.className='status error';return}
 const value=input.type==='checkbox'?(input.checked?'true':'false'):input.value;
 button.disabled=true;status.textContent='Updating...';status.className='status';
)HTML"));

    // JavaScript - chunk 3
    out.print(F(R"HTML(
 try{
  const body=new URLSearchParams({service:button.dataset.service,field:button.dataset.field,value});
  const response=await fetch('/config/update',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:body.toString(),cache:'no-store'});
  const reply=await response.text();
  if(response.ok){
   current.textContent=input.type==='checkbox'?(reply==='true'?'Yes':'No'):reply;
   if(input.type==='checkbox')input.checked=reply==='true';else input.value=reply;
   status.textContent='Updated successfully';status.className='status ok';
)HTML"));

    // JavaScript - chunk 4
    out.print(F(R"HTML(
   const timer=setTimeout(()=>{status.textContent='';status.className='status';successTimers.delete(row)},5000);
   successTimers.set(row,timer);
  }else{status.textContent=reply||'Update refused';status.className='status error'}
 }catch(e){status.textContent='Communication error';status.className='status error'}
 finally{button.disabled=false}
}
)HTML"));

    // JavaScript - chunk 5
    out.print(F(R"HTML(
document.addEventListener('DOMContentLoaded',()=>{
 document.querySelectorAll('.new-value input').forEach(input=>input.addEventListener('input',()=>{
 const row=input.closest('tr');clearSuccessTimer(row);
 const status=row.querySelector('.status');status.textContent='';status.className='status';
}));
});
</script>
</head><body><h1>Service Configuration</h1><div class="tabs">
)HTML"));
    out.print(F("<script>"));
    out.print(F("function showTab(index){"));
    out.print(F("document.querySelectorAll('.tab').forEach(t=>t.classList.toggle('active',Number(t.dataset.service)===index));"));
    out.print(F("document.querySelectorAll('.panel').forEach(p=>p.classList.toggle('active',p.id==='panel'+index));"));
    out.print(F("}"));
    out.print(F("</script>"));
    bool first = true;
    for (size_t s = 0; s < numServices; ++s)
    {
        if (!services[s])
            continue;
        out.print(F("<button type='button' class='tab"));
        if (first)
            out.print(F(" active"));
        out.print(F("' data-service='"));
        out.print(s);
        out.print(F("' onclick='showTab("));
        out.print(s);
        out.print(F(")'>"));
        _html_escape(out, services[s]->get_name());
        out.println(F("</button>"));
        first = false;
    }
    out.println(F("</div>"));
    first = true;
    for (size_t s = 0; s < numServices; ++s)
    {
        if (!services[s])
            continue;
        out.print(F("<section class='panel"));
        if (first)
            out.print(F(" active"));
        out.print(F("' id='panel"));
        out.print(s);
        out.println(F("'>"));
        first = false;
        out.print(F("<h2>"));
        _html_escape(out, services[s]->get_name());
        out.println(F("</h2>"));
        out.println(F("<table><thead><tr><th>Parameter</th><th>Current Value</th><th>New Value</th><th>Action</th></tr></thead><tbody>"));
        ConfigFieldList fields;
        services[s]->get_config_fields(fields);
        for (size_t i = 0; i < fields.count; ++i)
        {
            const ConfigField &f = fields.fields[i];
            out.print(F("<tr><td class='parameter'>"));
            _html_escape(out, f.label);
            if (f.hasRange)
            {
                char limits[128];
                if (f.type == ConfigFieldType::Integer)
                    snprintf(limits, sizeof(limits), "%" PRId32 " - %" PRId32 ", step %" PRId32, f.intMin, f.intMax, f.intStep);
                else
                    snprintf(limits, sizeof(limits), "%.6g - %.6g, step %.6g", f.floatMin, f.floatMax, f.floatStep);
                out.print(F("<span class='limits'>"));
                _html_escape(out, limits);
                out.print(F("</span>"));
            }
            out.print(F("</td><td class='current'>"));
            if (f.type == ConfigFieldType::Boolean)
                out.print(!strcmp(f.displayValue, "true") ? F("Yes") : F("No"));
            else
                _html_escape(out, f.displayValue);
            out.print(F("</td><td class='new-value'>"));
            if (f.type == ConfigFieldType::Boolean)
            {
                out.print(F("<input type='checkbox'"));
                if (!strcmp(f.displayValue, "true"))
                    out.print(F(" checked"));
                out.print(F(">"));
            }
            else
            {
                out.print(f.type == ConfigFieldType::Text ? F("<input type='text'") : F("<input type='number'"));
                if (f.hasRange)
                {
                    char attr[64];
                    if (f.type == ConfigFieldType::Integer)
                    {
                        snprintf(attr, sizeof(attr), "%" PRId32, f.intMin);
                        out.print(F(" min='"));
                        out.print(attr);
                        snprintf(attr, sizeof(attr), "%" PRId32, f.intMax);
                        out.print(F("' max='"));
                        out.print(attr);
                        snprintf(attr, sizeof(attr), "%" PRId32, f.intStep);
                        out.print(F("' step='"));
                        out.print(attr);
                    }
                    else
                    {
                        snprintf(attr, sizeof(attr), "%.8g", f.floatMin);
                        out.print(F(" min='"));
                        out.print(attr);
                        snprintf(attr, sizeof(attr), "%.8g", f.floatMax);
                        out.print(F("' max='"));
                        out.print(attr);
                        snprintf(attr, sizeof(attr), "%.8g", f.floatStep);
                        out.print(F("' step='"));
                        out.print(attr);
                    }
                    out.print(F("'"));
                }
                out.print(F(" value=\""));
                _html_escape(out, f.displayValue);
                out.print(F("\">"));
            }
            out.print(F("</td><td class='action'><button type='button' class='update' data-service='"));
            out.print(s);
            out.print(F("' data-field=\""));
            _html_escape(out, f.id);
            out.print(F("\" onclick='updateField(this)'>Update</button>"));
            out.println(F("<span class='status' role='status' aria-live='polite'></span></td></tr>"));
        }
        out.println(F("</tbody></table></section>"));
    }
    out.println(F(R"HTML(</body></html>)HTML"));
    out.println(F("<!-- END_OF_CONFIG_PAGE -->"));
}

void WebpageService::_generate_default_config()
{
    _config.data.maxClients = 4;
    _config.data.serverPort = 80;
}

void WebpageService::_print_config()
{
    _info("Config:");
    _infof("\tServer Port: %d", _config.data.serverPort);
    _infof("\tMaximum number of clients: %d", _config.data.maxClients);
}

void WebpageService::_html_escape(Print &out, const char *outputStr)
{
    if (!outputStr)
        return;
    while (*outputStr)
    {
        switch (*outputStr)
        {
        case '&':
            out.print(F("&amp;"));
            break;
        case '<':
            out.print(F("&lt;"));
            break;
        case '>':
            out.print(F("&gt;"));
            break;
        case '"':
            out.print(F("&quot;"));
            break;
        case '\'':
            out.print(F("&#39;"));
            break;
        default:
            out.write((uint8_t)*outputStr);
        }
        ++outputStr;
    }
}

void WebpageService::_send_response(EthernetClient &client, int code, const char *reason, const char *content)
{
    client.print(F("HTTP/1.1 "));
    client.print(code);
    client.print(' ');
    client.println(reason);
    client.println(F("Content-Type: text/plain; charset=utf-8"));
    client.println(F("Cache-Control: no-store"));
    client.println(F("Connection: close"));
    client.print(F("Content-Length: "));
    client.println(strlen(content));
    client.println();
    client.print(content);
}

bool WebpageService::_read_line(EthernetClient &client, char *buf, size_t capacity)
{
    if (!capacity)
        return false;
    size_t len = 0;
    unsigned long last = millis();
    while (millis() - last < 2000)
    {
        if (!client.available())
        {
            if (!client.connected())
                return false;
            delay(1);
            continue;
        }
        int c = client.read();
        last = millis();
        if (c == '\n')
        {
            if (len && buf[len - 1] == '\r')
                --len;
            buf[len] = 0;
            return true;
        }
        if (!c || len + 1 >= capacity)
            return false;
        buf[len++] = (char)c;
    }
    return false;
}

void WebpageService::_handle_client(EthernetClient &client)
{
    char line[HTTP_LINE_SIZE];
    if (!_read_line(client, line, sizeof(line)))
        return;
    bool getPage = !strcmp(line, "GET / HTTP/1.1") || !strcmp(line, "GET / HTTP/1.0");
    bool post = !strcmp(line, "POST /config/update HTTP/1.1") ||
                !strcmp(line, "POST /config/update HTTP/1.0");
    if (!getPage && !post)
    {
        _send_response(client, 404, "Not Found", "Not found");
        return;
    }
    size_t contentLength = 0;
    bool haveLength = false, validType = false, unsupported = false, complete = false;
    for (size_t i = 0; i < 24; ++i)
    {
        if (!_read_line(client, line, sizeof(line)))
            return;
        if (!line[0])
        {
            complete = true;
            break;
        }
        for (char *q = line; *q; ++q)
            if (*q >= 'A' && *q <= 'Z')
                *q = *q - 'A' + 'a';
        if (!strncmp(line, "content-length:", 15))
        {
            if (haveLength)
            {
                _send_response(client, 400, "Bad Request", "Duplicate Content-Length");
                return;
            }
            const char *p = line + 15;
            while (*p == ' ' || *p == '\t')
                ++p;
            if (!_parse_unsigned(p, contentLength))
            {
                _send_response(client, 400, "Bad Request", "Invalid Content-Length");
                return;
            }
            haveLength = true;
        }
        if (!strncmp(line, "content-type:", 13))
        {
            const char *p = line + 13;
            while (*p == ' ' || *p == '\t')
                ++p;
            const char *expected = "application/x-www-form-urlencoded";
            size_t n = strlen(expected);
            if (!strncmp(p, expected, n) && (p[n] == 0 || p[n] == ';'))
                validType = true;
        }
        if (!strncmp(line, "transfer-encoding:", 18))
            unsupported = true;
    }
    if (!complete || unsupported)
    {
        _send_response(client, 400, "Bad Request", "Unsupported headers");
        return;
    }
    if (getPage)
    {
        client.println(F("HTTP/1.1 200 OK"));
        client.println(F("Content-Type: text/html; charset=utf-8"));
        client.println(F("Cache-Control: no-store"));
        client.println(F("Connection: close"));
        client.println();
        generate_config_page(client);
        return;
    }
    if (!haveLength || !validType || !contentLength || contentLength > HTTP_BODY_SIZE)
    {
        _send_response(client, 400, "Bad Request", "Invalid update request");
        return;
    }
    char body[HTTP_BODY_SIZE + 1];
    size_t received = client.readBytes(body, contentLength);
    if (received != contentLength)
    {
        _send_response(client, 400, "Bad Request", "Incomplete request");
        return;
    }
    body[contentLength] = 0;
    if (strlen(body) != contentLength)
    {
        _send_response(client, 400, "Bad Request", "Invalid request body");
        return;
    }
    size_t index = 0;
    char id[FIELD_ID_BUFFER_SIZE] = {}, value[DISPLAY_VALUE_BUFFER_SIZE] = {};
    if (!parseUpdateBody(body, index, id, sizeof(id), value, sizeof(value)))
    {
        _send_response(client, 400, "Bad Request", "Malformed update request");
        return;
    }
    char message[MESSAGE_SIZE] = {}, accepted[DISPLAY_VALUE_BUFFER_SIZE] = {};
    ServiceId serviceIdToUpdate = ServiceId::_values()[index];

    BaseService *service = _ctx->serviceHelper->get_service(serviceIdToUpdate);

    bool ok = service->apply_config_update(id, value,
                                           message, sizeof(message), accepted, sizeof(accepted));
    if (ok)
        _send_response(client, 200, "OK", accepted);
    else
        _send_response(client, 422, "Unprocessable Content", message);
}

bool WebpageService::_parse_unsigned(const char *p, size_t &result)
{
    if (!p || !*p)
        return false;
    size_t value = 0;
    while (*p)
    {
        if (*p < '0' || *p > '9')
            return false;
        size_t digit = (size_t)(*p - '0');
        if (value > (SIZE_MAX - digit) / 10)
            return false;
        value = value * 10 + digit;
        ++p;
    }
    result = value;
    return true;
}
bool WebpageService::parseUpdateBody(const char *body, size_t &serviceIndex, char *id, size_t idSize,
                                     char *value, size_t valueSize)
{
    bool hasService = false, hasField = false, hasValue = false;
    const char *p = body;
    while (*p)
    {
        const char *end = strchr(p, '&');
        if (!end)
            end = p + strlen(p);
        const char *eq = (const char *)memchr(p, '=', (size_t)(end - p));
        if (!eq)
            return false;
        char key[24], decoded[HTTP_BODY_SIZE + 1];
        if (!_url_decode(p, (size_t)(eq - p), key, sizeof(key)) ||
            !_url_decode(eq + 1, (size_t)(end - eq - 1), decoded, sizeof(decoded)))
            return false;
        if (!strcmp(key, "service"))
        {
            if (hasService || !_parse_unsigned(decoded, serviceIndex))
                return false;
            hasService = true;
        }
        else if (!strcmp(key, "field"))
        {
            if (hasField || !*decoded || strlen(decoded) >= idSize)
                return false;
            for (const char *q = decoded; *q; ++q)
            {
                char c = *q;
                if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                      (c >= '0' && c <= '9') || c == '_'))
                    return false;
            }
            strcpy(id, decoded);
            hasField = true;
        }
        else if (!strcmp(key, "value"))
        {
            if (hasValue || strlen(decoded) >= valueSize)
                return false;
            strcpy(value, decoded);
            hasValue = true;
        }
        else
            return false;
        if (!*end)
            break;
        p = end + 1;
    }
    return hasService && hasField && hasValue;
}

int WebpageService::_hex_digit(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}
bool WebpageService::_url_decode(const char *src, size_t length, char *dst, size_t capacity)
{
    if (!dst || !capacity)
        return false;
    size_t out = 0;
    for (size_t i = 0; i < length; ++i)
    {
        unsigned char c = (unsigned char)src[i];
        if (c == '+')
            c = ' ';
        else if (c == '%')
        {
            if (i + 2 >= length)
                return false;
            int hi = _hex_digit(src[i + 1]), lo = _hex_digit(src[i + 2]);
            if (hi < 0 || lo < 0)
                return false;
            c = (unsigned char)((hi << 4) | lo);
            i += 2;
        }
        if (c < 32 || c == 127 || out + 1 >= capacity)
            return false;
        dst[out++] = (char)c;
    }
    dst[out] = 0;
    return true;
}
