/******************************************************************************/
/*!
\file		LoggerSystem.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	February 15, 2024
\brief		Declares the functions for Logger System of the Borealis

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/

#ifndef LOGGER_SYSTEM_HPP
#define LOGGER_SYSTEM_HPP

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <Core/Core.hpp>
#include <Debugging/Console.hpp>

// Define the log levels
#define ENGINE_LOGLEVEL_INFO spdlog::level::info
#define ENGINE_LOGLEVEL_TRACE spdlog::level::trace
#define ENGINE_LOGLEVEL_WARN spdlog::level::warn
#define ENGINE_LOGLEVEL_ERROR spdlog::level::err
#define ENGINE_LOGLEVEL_CRITICAL spdlog::level::critical

#ifdef BOREALIS_ENABLE_ASSERTS
#include <filesystem>
#define BOREALIS_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { BOREALIS##type##ERROR(msg, __VA_ARGS__); BOREALIS_DEBUGBREAK(); } }
#define BOREALIS_INTERNAL_ASSERT_WITH_MSG(type, check, ...) BOREALIS_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define BOREALIS_INTERNAL_ASSERT_NO_MSG(type, check) BOREALIS_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", BOREALIS_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define BOREALIS_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define BOREALIS_INTERNAL_ASSERT_GET_MACRO(...) BOREALIS_EXPAND_MACRO( BOREALIS_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, BOREALIS_INTERNAL_ASSERT_WITH_MSG, BOREALIS_INTERNAL_ASSERT_NO_MSG) )

// Currently accepts at least the condition and one additional parameter (the message) being optional
#define BOREALIS_ASSERT(...) BOREALIS_EXPAND_MACRO( BOREALIS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#define BOREALIS_CORE_ASSERT(...) BOREALIS_EXPAND_MACRO( BOREALIS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
#define BOREALIS_ASSERT(...)
#define BOREALIS_CORE_ASSERT(...)
#endif

namespace Borealis {

	

	/*!***********************************************************************
		\brief
			Class for Logger System of the Borealis
	*************************************************************************/
	class BOREALIS_API LoggerSystem
	{
	public:
		/*!***********************************************************************
			\brief
				Initializes the Logger System
			\param[in] level
				The log level to be set for the Borealis Logger
		*************************************************************************/
		static void Init(spdlog::level::level_enum = ENGINE_LOGLEVEL_TRACE);

		/*!***********************************************************************
			\brief
				Shuts down the Logger System
		*************************************************************************/
		static void Shutdown();

		/*!***********************************************************************
			\brief
				Returns the Borealis Logger
			\return
				The Borealis Logger
		*************************************************************************/
		inline static spdlog::logger*& GetEngineLogger() { return sEngineLogger; }

		/*!***********************************************************************
			\brief
				Returns the Application Logger
			\return
				The Application Logger
		*************************************************************************/
		inline static spdlog::logger*& GetApplicationLogger() { return sApplicationLogger; }

		/*!***********************************************************************
			\brief
				Sets the log level for the Borealis Logger
			\param[in] level
				The log level to be set
		*************************************************************************/
		void SetEngineLogLevel(spdlog::level::level_enum level);

		/*!***********************************************************************
			\brief
				Sets the log level for the Application Logger
			\param[in] level
				The log level to be set
		*************************************************************************/
		void SetApplicationLogLevel(spdlog::level::level_enum level);

		static std::string GetString() { return sOSS.str(); }

		static void Flush() { 
			std::ostringstream().swap(sOSS);
		}

	private:
		static spdlog::logger* sEngineLogger; // The Borealis Logger
		static spdlog::logger* sApplicationLogger; // The Application Logger
		static std::ostringstream sOSS;
	}; // End of Class LoggerSystem

} // End of namespace Borealis
 
// Define the log macros
#define BOREALIS_CORE_INFO(...)     ::Borealis::LoggerSystem::GetEngineLogger()->info(__VA_ARGS__); ::Borealis::Console::GetInstance().Log(::Borealis::LoggerSystem::GetString(), ::Borealis::Console::INFO_MESSAGE); ::Borealis::LoggerSystem::Flush()
#define BOREALIS_CORE_TRACE(...)    ::Borealis::LoggerSystem::GetEngineLogger()->trace(__VA_ARGS__);  ::Borealis::Console::GetInstance().Log(::Borealis::LoggerSystem::GetString(), ::Borealis::Console::INFO_MESSAGE); ::Borealis::LoggerSystem::Flush()
#define BOREALIS_CORE_WARN(...)     ::Borealis::LoggerSystem::GetEngineLogger()->warn(__VA_ARGS__); ::Borealis::Console::GetInstance().Log(::Borealis::LoggerSystem::GetString(), ::Borealis::Console::WARNING_MESSAGE); ::Borealis::LoggerSystem::Flush()
#define BOREALIS_CORE_ERROR(...)    ::Borealis::LoggerSystem::GetEngineLogger()->error(__VA_ARGS__); ::Borealis::Console::GetInstance().Log(::Borealis::LoggerSystem::GetString(), ::Borealis::Console::ERROR_MESSAGE); ::Borealis::LoggerSystem::Flush()
#define BOREALIS_CORE_CRITICAL(...) ::Borealis::LoggerSystem::GetEngineLogger()->critical(__VA_ARGS__); ::Borealis::Console::GetInstance().Log(::Borealis::LoggerSystem::GetString(), ::Borealis::Console::ERROR_MESSAGE); ::Borealis::LoggerSystem::Flush()

#define APP_LOG_INFO(...)        ::Borealis::LoggerSystem::GetApplicationLogger()->info(__VA_ARGS__)
#define APP_LOG_TRACE(...)       ::Borealis::LoggerSystem::GetApplicationLogger()->trace(__VA_ARGS__)
#define APP_LOG_WARN(...)        ::Borealis::LoggerSystem::GetApplicationLogger()->warn(__VA_ARGS__)
#define APP_LOG_ERROR(...)       ::Borealis::LoggerSystem::GetApplicationLogger()->error(__VA_ARGS__)
#define APP_LOG_CRITICAL(...)    ::Borealis::LoggerSystem::GetApplicationLogger()->critical(__VA_ARGS__)

#endif