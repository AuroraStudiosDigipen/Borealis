/******************************************************************************/
/*!
\file		Profiler.cpp
\author 	Liu Chengrong
\par    	email: chengrong.liu\@digipen.edu
\date   	July 10, 2024
\brief		Implements the TracyProfiler class methods for profiling, logging, and memory tracking.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <Debugging/Profiler.hpp>
#include <Tracy.hpp>
#include <common/TracySystem.hpp>
#include <cstring>  


//Tracy Profiler (https://github.com/wolfpld/tracy)

namespace Borealis
{
    size_t Borealis::TracyProfiler::totalAllocatedMemory = 0;

    // Records a custom plot value in Tracy with a given plot name.
    void TracyProfiler::recordPlot(const char* plotName, float value) {
        TracyPlot(plotName, value); 
        
    }

    // Logs a message in Tracy profiler.
    void TracyProfiler::logMessage(const char* message) {
        TracyMessage(message, strlen(message));
    }

    // Logs a colored message in Tracy profiler using a vec4 color.
    void TracyProfiler::logMessageColored(const char* message, const glm::vec4& color) {
        uint32_t packedColor = vec4ToColor(color);  // Convert vec4 to packed uint32_t color
        TracyMessageC(message, strlen(message), packedColor);  
    }

    // Tracks a memory allocation in Tracy profiler.
    void TracyProfiler::trackAllocation(void* ptr, size_t size) {
        TracyAlloc(ptr, size);  // Track memory allocation in Tracy

        totalAllocatedMemory += size;

        recordPlot("testing", totalAllocatedMemory);
    }

    // Tracks a memory free operation in Tracy profiler.
    void TracyProfiler::trackFree(void* ptr) {
        TracyFree(ptr); 
    }

    // Tracks a secure memory allocation in Tracy profiler.
    void TracyProfiler::trackSecureAllocation(void* ptr, size_t size) {
        TracySecureAlloc(ptr, size); 
    }

    // Tracks a secure memory free operation in Tracy profiler.
    void TracyProfiler::trackSecureFree(void* ptr) {
        TracySecureFree(ptr);  
    }

    // Sends custom application information to the Tracy profiler.
    void TracyProfiler::sendAppInfo(const char* message) {
        TracyAppInfo(message, strlen(message)); 
    }

    // Mark the frame boundary, Help track overall performance at the frame level
    void TracyProfiler::markFrame(const char* frameName) {
        if (frameName) {
            FrameMarkNamed(frameName);  // Mark a frame with a custom name
        }
        else {
            FrameMark;  // Use the default frame marker
        }
    }

    // Function marks the start of a frame with a specific name
    void TracyProfiler::markFrameStart(const char* frameName)
    {
        FrameMarkStart(frameName);
    }

    // Function marks the end of a custom frame that was started using framemarkstart
    void TracyProfiler::markFrameEnd(const char* frameName)
    {
        FrameMarkEnd(frameName);
    }
    
    // Start a custom profiling zone, provides detailed insight into those frame
    void TracyProfiler::startZone(const mySourceLocationData* loc) {
        tracy::ScopedZone ___tracy_scoped_zone(reinterpret_cast<const tracy::SourceLocationData*>(loc), true);
    }
    
    // Destructor that ends the profiling zone
    TracyProfiler::~TracyProfiler() {
    }

    // Converts a glm::vec4 color to a packed uint32_t format (0xAARRGGBB).
    uint32_t TracyProfiler::vec4ToColor(const glm::vec4& color) {
        uint8_t r = static_cast<uint8_t>(color.r * 255.0f);
        uint8_t g = static_cast<uint8_t>(color.g * 255.0f);
        uint8_t b = static_cast<uint8_t>(color.b * 255.0f);
        uint8_t a = static_cast<uint8_t>(color.a * 255.0f);

        // Pack color as 0xAARRGGBB
        return (a << 24) | (r << 16) | (g << 8) | b;
    }

    void TracyProfiler::logInfo(const char* message)
    {
        size_t messageLength = strlen(message);
        TracyMessageC(message, messageLength,0x00FFFFFF);
    }
    void TracyProfiler::logWarning(const char* message)
    {
        size_t messageLength = strlen(message);
        TracyMessageC(message, messageLength, 0x00FFFF00);
    }
    void TracyProfiler::logError(const char* message)
    {
        size_t messageLength = strlen(message);
        TracyMessageC(message, messageLength, 0x00FF0000);
    }

    void TracyProfiler::toggleProfiler()
    {
        TracyProfiler::g_EnableProfiler = !TracyProfiler::g_EnableProfiler;
        if (TracyProfiler::g_EnableProfiler) {
            std::cout << "Profiler Enabled" << std::endl;
        }
        else {
            std::cout << "Profiler Disabled" << std::endl;
        }
    }


    Timer::~Timer()
    {
        double time_run = std::chrono::duration<double, std::milli>((std::chrono::high_resolution_clock::now() - mTime)).count();
        TracyPlot(mName.c_str(), time_run);
    }

    void MemoryTracker::TrackAllocation(void* ptr, size_t size) {
        m_memoryAllocated += size;
        m_allocations[ptr] = size; // Store size for accurate deallocation tracking
        TracyAlloc(ptr, size); // Pass actual size
    }

    void MemoryTracker::TrackDeallocation(void* ptr) {
        auto it = m_allocations.find(ptr);
        if (it != m_allocations.end()) {
            m_memoryAllocated -= it->second;
            TracyFree(ptr);
            m_allocations.erase(it); // Remove entry after deallocation
        }
    }

    void MemoryTracker::PlotMemory() {
        TracyPlot(("Memory Usage: " + m_name).c_str(), static_cast<double>(m_memoryAllocated));
    }



}