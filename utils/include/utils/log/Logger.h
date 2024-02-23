#ifndef MD_2024_LOGGER_H
#define MD_2024_LOGGER_H

#include "spdlog/spdlog.h"
#include "spdlog/stopwatch.h"

class Logger {
private:
    static std::shared_ptr<spdlog::logger> logger;

public:
    static void init();
    static std::shared_ptr<spdlog::logger>& getLogger() { return logger; }

};

#define TET_TRACE(...)    ::Logger::getLogger()->trace(__VA_ARGS__)
#define TET_INFO(...)     ::Logger::getLogger()->info(__VA_ARGS__)
#define TET_WARN(...)     ::Logger::getLogger()->warn(__VA_ARGS__)
#define TET_ERROR(...)    ::Logger::getLogger()->error(__VA_ARGS__)
#define TET_CRITICAL(...) do{ \
::Logger::getLogger()->critical(__VA_ARGS__); \
throw std::exception("A critical error occurred!"); \
} while (false)

#endif //MD_2024_LOGGER_H
