/******************************************************************************/
/*!
\file		LoggerSystem.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	February 15, 2024
\brief		Defines the functions for Logger System of the Borealis

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <spdlog\sinks\stdout_color_sinks.h>
#include <Core/LoggerSystem.hpp>
#include <spdlog\sinks\ostream_sink.h>

namespace Borealis 
{
	spdlog::logger* LoggerSystem::sEngineLogger;
	spdlog::logger* LoggerSystem::sApplicationLogger;
	std::ostringstream LoggerSystem::sOSS;


	/*!***********************************************************************
		\brief
			Initializes the Logger System
		\param[in] level
			The log level to be set for the Borealis Logger
	*************************************************************************/
	void LoggerSystem::Init(spdlog::level::level_enum level)
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		sEngineLogger = spdlog::stdout_color_mt("ENGINE").get();
		sEngineLogger->set_level(level);

		sApplicationLogger = spdlog::stdout_color_mt("APP").get();
		sApplicationLogger->set_level(level);

		sApplicationLogger->sinks().push_back(std::make_shared<spdlog::sinks::ostream_sink_mt>(sOSS));
		sEngineLogger->sinks().push_back(std::make_shared<spdlog::sinks::ostream_sink_mt>(sOSS));
	}


	/*!***********************************************************************
		\brief
			Shuts down the Logger System
	*************************************************************************/
	void LoggerSystem::Shutdown()
	{
		spdlog::shutdown();
	}

	/*!***********************************************************************
		\brief
			Sets the log level for the Borealis Logger
		\param[in] level
			The log level to be set
	*************************************************************************/
	void LoggerSystem::SetEngineLogLevel(spdlog::level::level_enum level)
	{
		sEngineLogger->set_level(level);
	}

	/*!***********************************************************************
		\brief
			Sets the log level for the Application Logger
		\param[in] level
			The log level to be set
	*************************************************************************/
	void LoggerSystem::SetApplicationLogLevel(spdlog::level::level_enum level)
	{
		sApplicationLogger->set_level(level);
	}
} // End of namespace Borealis