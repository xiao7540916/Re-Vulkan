#include "LogManager.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Horizon
{
	LoggerSharedPtr LogManager::kLogger = nullptr;

	void LogManager::Init()
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Horizon.log", true));
		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");
		kLogger = std::make_shared<spdlog::logger>("logger", begin(logSinks), end(logSinks));
		spdlog::register_logger(kLogger);
		kLogger->set_level(spdlog::level::trace);
		kLogger->flush_on(spdlog::level::trace);
	}
}
