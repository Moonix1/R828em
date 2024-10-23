#pragma once

#include <memory>
#include <stdio.h>

#include <spdlog/spdlog.h>

namespace RASM {

	class Log {
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }
	private:
		static std::shared_ptr<spdlog::logger> s_Logger;
	};

}

#define TRACE(...)		::RASM::Log::GetLogger()->trace(__VA_ARGS__)
#define INFO(...)		::RASM::Log::GetLogger()->info(__VA_ARGS__)
#define WARN(...)		::RASM::Log::GetLogger()->warn(__VA_ARGS__)
#define ERROR(...)		::RASM::Log::GetLogger()->error(__VA_ARGS__)