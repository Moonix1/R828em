#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace RASM {

	std::shared_ptr<spdlog::logger> Log::s_Logger;
	
	void Log::Init() {
		spdlog::set_pattern("%^[%l]: %v%$");
		s_Logger = spdlog::stdout_color_mt("MAGNO_COMPILER");
		s_Logger->set_level(spdlog::level::trace);
	}

}