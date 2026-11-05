#include "ServiceHelper.h"
#include "BaseService.h"

ServiceHelper::ServiceHelper(HelperContext *ctx) : _ctx(ctx)
{
    _serviceList = (BaseService **)malloc((sizeof(BaseService *) * get_num_services()));
    for (int i = 0; i < get_num_services(); i++)
    {
        _serviceList[i] = nullptr;
    }
}
void ServiceHelper::register_services(BaseService **serviceList, size_t len)
{
    for (int i = 0; i < len; i++)
    {
        ServiceId id = serviceList[i]->getServiceId();
        int index = id._to_integral();
        if (_serviceList[index] != nullptr)
        {
            _ctx->loggingHelper->error("Service Helper", "Attempted to create multiple instances of the same service");
        }
        _serviceList[index] = serviceList[i];
    }
}

BaseService *ServiceHelper::get_service(ServiceId serviceId)
{
    int index = serviceId._to_integral();
    BaseService *ret = _serviceList[index];
    if (ret == nullptr)
    {
        _ctx->loggingHelper->error("Service Helper", "Requested information on non-existent service");
    }
    return ret;
}

void ServiceHelper::tick()
{
    for (int i = 0; i < get_num_services(); i++)
    {
        if (_serviceList[i] != nullptr)
            _serviceList[i]->tick();
    }
}

BaseService **ServiceHelper::get_service_list()
{
    return _serviceList;
}

int ServiceHelper::get_num_services()
{
    return ServiceId::_size();
}
