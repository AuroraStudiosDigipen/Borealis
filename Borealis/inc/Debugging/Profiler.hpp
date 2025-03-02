/******************************************************************************/
/*!
\file		Profiler.hpp
\author 	Liu Chengrong
\par    	email: chengrong.liu\@digipen.edu
\date   	July 10, 2024
\brief		Declares the classes for profiling tools

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/


#define ENGINE_PROFILE 0

#ifndef PROFILER_HPP
#define PROFILER_HPP

#define CONCATENATE_IMPL(a, b) a##b
#define CONCATENATE(a, b) CONCATENATE_IMPL(a, b)

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <thread>
#include <glm/glm.hpp>

#include <unordered_map>

namespace Borealis
{
	struct mySourceLocationData
	{
		const char* name;		//name of profiling zone
		const char* function;	//function name of where profiling is being used
		const char* file;		//file name of where profiling is being used
		uint32_t line;			//line number in the file
		uint32_t color;			//color of profiling zone
	};

	class TracyProfiler {
	public:
		// Default constructor
		TracyProfiler() = delete;

		/***********************************************************
		 * @brief Record a custom plot value.
		 * @param plotName The name of the custom plot.
		 * @param value The value to record.
		***********************************************************/
		static void recordPlot(const char* plotName, float value);

		/***********************************************************
		* @brief Log a message in the profiler.
		* @param message The message to log.
		***********************************************************/
		static void logMessage(const char* message);

		/***********************************************************
		* @brief Log a message with a specific color in the profiler.
		* @param message The message to log.
		* @param color The color for the message, encoded as a glm::vec4 (RGBA).
		***********************************************************/
		static void logMessageColored(const char* message, const glm::vec4& color);

		/***********************************************************
		* @brief Track a memory allocation.
		* @param ptr A pointer to the allocated memory.
		* @param size The size of the allocated memory in bytes.
		***********************************************************/
		static void trackAllocation(void* ptr, size_t size);

		/***********************************************************
		* @brief Track memory being freed.
		* @param ptr A pointer to the memory to be freed.
		***********************************************************/
		static void trackFree(void* ptr);

		/***********************************************************
		 * @brief Track a secure memory allocation (e.g., for sensitive data).
		 * @param ptr A pointer to the allocated memory.
		 * @param size The size of the allocated memory in bytes.
		***********************************************************/
		static void trackSecureAllocation(void* ptr, size_t size);

		/***********************************************************
		* @brief Track secure memory being freed.
		* @param ptr A pointer to the secure memory to be freed.
		***********************************************************/
		static void trackSecureFree(void* ptr);

		/***********************************************************
		* @brief Mark the boundary of a frame in the application.
		* @param frameName Optional name of the frame.
		***********************************************************/
		static void markFrame(const char* frameName = nullptr);

		/***********************************************************
		* @brief Mark the start of a frame in the application.
		* @param frameName Optional name of the frame.
		***********************************************************/
		static void markFrameStart(const char* frameName = nullptr);

		/***********************************************************
		* @brief Mark the end of a frame in the application.
		* @param frameName Optional name of the frame.
		***********************************************************/
		static void markFrameEnd(const char* frameName = nullptr);

		/***********************************************************
		* @brief Send application information to the profiler.
		* @param message The information message to send.
		***********************************************************/
		static void sendAppInfo(const char* message);

		/***********************************************************
		* @brief Start a custom profiling zone.
		* @param srcLoc A pointer to the mySourceLocationData struct 
		   containing information about the profiling zone.
		***********************************************************/
		static void startZone(const mySourceLocationData* srcLoc);

		// Destructor that ends the profiling zone
		~TracyProfiler();

		/***********************************************************
		* @brief Convert a glm::vec4 color to a 32-bit unsigned integer representation.
		* @param color The glm::vec4 color to convert (RGBA format).
		* @return The color encoded as a 32-bit unsigned integer.
		***********************************************************/
		static uint32_t vec4ToColor(const glm::vec4& color);

		static void logInfo(const char* message);
		static void logWarning(const char* message);
		static void logError(const char* message);

		static void toggleProfiler();

		inline static bool g_EnableProfiler = false;

	private:
		// Disable copying
		TracyProfiler(const TracyProfiler&) = delete;
		TracyProfiler& operator=(const TracyProfiler&) = delete;

		static size_t totalAllocatedMemory;
	};

	class Timer {
	public:
		Timer(std::string name) : mTime(std::chrono::high_resolution_clock::now()), mName(name){}
		~Timer();
		
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> mTime;
		std::string mName;
	};

	class MemoryTracker {
	public:
		MemoryTracker(const std::string& name)
			: m_name(name), m_memoryAllocated(0) {}

		void TrackAllocation(void* ptr, size_t size);

		void TrackDeallocation(void* ptr);

		void PlotMemory();

	private:
		std::string m_name;
		size_t m_memoryAllocated;

		std::unordered_map<void*, size_t> m_allocations;
	};

}

// Macros for profiling
#if ENGINE_PROFILE
#define PROFILE_START(name) \
    if (Borealis::TracyProfiler::g_EnableProfiler) ::Borealis::TracyProfiler::markFrameStart(name)
#define PROFILE_END() \
    if (Borealis::TracyProfiler::g_EnableProfiler) ::Borealis::TracyProfiler::markFrameEnd()
#define PROFILE_SCOPE(name) \
    if (Borealis::TracyProfiler::g_EnableProfiler) { \
        static Borealis::mySourceLocationData CONCATENATE(__source_location,__LINE__) { name, __FUNCTION__, __FILE__, (uint32_t)__LINE__, 0 }; \
        ::Borealis::TracyProfiler::startZone(&CONCATENATE(__source_location,__LINE__)); \
    }
#define PROFILE_FUNCTION() \
    if (Borealis::TracyProfiler::g_EnableProfiler) { \
        static Borealis::mySourceLocationData CONCATENATE(__source_location,__LINE__) { nullptr, __FUNCTION__, __FILE__, (uint32_t)__LINE__, 0 }; \
        ::Borealis::TracyProfiler::startZone(&CONCATENATE(__source_location,__LINE__)); \
    }
#else
#define PROFILE_START(name)
#define PROFILE_END()
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION()
#endif


#endif