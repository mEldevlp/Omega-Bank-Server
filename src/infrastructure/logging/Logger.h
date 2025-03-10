#pragma once 

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h> 
#include <memory>

class Logger 
{
public:
    static std::shared_ptr<spdlog::logger> logger;

    static void init(const std::string& logger_name)
    {
        logger = spdlog::stdout_color_mt(logger_name);
        spdlog::set_default_logger(logger);
        spdlog::set_level(spdlog::level::debug);
    }

    static void Info(const std::string& msg)
    {
        logger->info(msg);
    }

    static void Critical(const std::string& msg)
    {
        logger->critical(msg);
    }

    static void Error(const std::string& msg)
    {
        logger->error(msg);
    }

    static std::shared_ptr<spdlog::logger>& getLogger() 
    {
        return logger;
    }
};