#ifndef MYLOG_H
#define MYLOG_H
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/qt_sinks.h"

class MyLog
{
public:
    MyLog()noexcept;
    bool init(spdlog::sinks_init_list&);
    void log(std::string&&);
private:
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink{nullptr};

    std::shared_ptr<spdlog::sinks::qt_color_sink_mt> qt_sink{nullptr};

    std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> file_sink{nullptr};

    std::shared_ptr<spdlog::logger> logger{nullptr};
};

#endif // MYLOG_H
