#pragma once
#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

/// Simple wrapper to initialize global loggers
class Logger
{
public:
    /// Call once at startup to configure sinks and format
    static void init(const std::string &logFile = "trade_simulator.log",
                     size_t maxFileSize = 1048576 * 5, // 5 MB
                     size_t maxFiles = 3)
    {
        try
        {
            // Console sink with color
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(spdlog::level::info);

            // Rotating file sink
            auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                logFile, maxFileSize, maxFiles);
            file_sink->set_level(spdlog::level::debug);

            std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
            auto logger = std::make_shared<spdlog::logger>("trade", begin(sinks), end(sinks));
            logger->set_level(spdlog::level::debug);
            logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

            spdlog::register_logger(logger);
            spdlog::set_default_logger(logger);
        }
        catch (const spdlog::spdlog_ex &ex)
        {
            std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        }
    }
};