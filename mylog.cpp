#include "mylog.h"
#include "spdlog/spdlog.h"
MyLog::MyLog()noexcept
{

}

bool MyLog::init(spdlog::sinks_init_list &list)
{
    if(logger == nullptr){
        logger = std::make_shared<spdlog::logger>("logger", list);
        logger->set_level(spdlog::level::debug);
        return true;
    }
    return false;
}

void MyLog::log(std::string &&msg)
{
    SPDLOG_LOGGER_DEBUG(logger, msg);
}



