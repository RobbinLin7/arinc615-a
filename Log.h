#ifndef LOG_H
#define LOG_H
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/logger.h"

#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/common.h"
#include "spdlog/sinks/rotating_file_sink.h"
class Log{
public:
    Log(std::string logFileName){
        console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::warn);
        console_sink->set_pattern("[multi_sink_example][%^%l%$] %v");

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFileName, true);
        file_sink->set_level(spdlog::level::trace);

        logger = new spdlog::logger("multi_sink", {console_sink, file_sink});
        logger->set_level(spdlog::level::debug);
        logger->warn("this should appear in both console and file");
        logger->info("this message should not appear in the console, only in the file");
    }
    ~Log(){
        //delete logger;
    }
    spdlog::logger* logger;
private:
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink;
    std::shared_ptr<spdlog::sinks::basic_file_sink_mt> file_sink;
};
#endif // LOG_H
