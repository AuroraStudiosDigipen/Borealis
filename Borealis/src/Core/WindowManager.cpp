/******************************************************************************/
/*!
\file		WindowManager.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	February 17, 2024
\brief		Defines the functions for Window Manager of the Borealis

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/

#include "BorealisPCH.hpp"
#include <FMOD/fmod.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <Core/LoggerSystem.hpp>
#include "Core/WindowManager.hpp"
#include <Core/InputSystem.hpp>
#include <Events/EventWindow.hpp>
#include <Events/EventInput.hpp>
#include <Graphics/OpenGL/GraphicsContextOpenGLImpl.hpp>

namespace Borealis
{
	static bool sGLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		BOREALIS_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	/*!***********************************************************************
		\brief
			Constructor for WindowManager
		\param[in] properties
			Properties of the window
	*************************************************************************/
	WindowManager::WindowManager(const WindowProperties& properties)
	{
		PROFILE_FUNCTION();

		Init(properties);
	}

	/*!***********************************************************************
		\brief
			Destructor for WindowManager
	*************************************************************************/
	WindowManager::~WindowManager()
	{
		PROFILE_FUNCTION();

		Shutdown();
	}

	/*!***********************************************************************
		\brief
			Updates the window
	*************************************************************************/
	void WindowManager::OnUpdate()
	{
		PROFILE_FUNCTION();

		glfwPollEvents();
		mContext->SwapBuffers();
	}

	/*!***********************************************************************
		\brief
			Sets the VSync
		\param[in] enabled
			Whether VSync is enabled
	*************************************************************************/
	void WindowManager::SetVSync(bool enabled)
	{
		PROFILE_FUNCTION();

		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		mData.mIsVSync = enabled;
	}

	/*!***********************************************************************
		\brief
			Gets the VSync
		\return
			True if VSync is enabled, false otherwise
	*************************************************************************/
	bool WindowManager::IsVSync() const
	{
		return mData.mIsVSync;
	}

	bool WindowManager::GetCursorVisibility()
	{
		int cursorMode = glfwGetInputMode((GLFWwindow*)mWindow, GLFW_CURSOR);
		return cursorMode == GLFW_CURSOR_NORMAL;
	}

	void WindowManager::SetCursorVisibility(bool visible)
	{
		if (visible)
		{
			glfwSetInputMode((GLFWwindow*)mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetInputMode((GLFWwindow*)mWindow, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);

		}
		else
		{
			glfwSetInputMode((GLFWwindow*)mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetInputMode((GLFWwindow*)mWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

		}
	}

	void WindowManager::ToggleFullScreen()
	{
		if (mData.mIsFullScreen)
		{
			glfwSetWindowMonitor((GLFWwindow*)mWindow, nullptr, mData.mPosX, mData.mPosY, mData.mWidth, mData.mHeight, 0);
			mData.mIsFullScreen = false;
		}
		else
		{
			int windowedWidth, windowedHeight;
			int windowedPosX, windowedPosY;
			glfwGetWindowPos((GLFWwindow*)mWindow, &windowedPosX, &windowedPosY);
			glfwGetWindowSize((GLFWwindow*)mWindow, &windowedWidth, &windowedHeight);
			mData.mPosX = windowedPosX; mData.mPosY = windowedPosY;
			mData.mWidth = windowedWidth; mData.mHeight = windowedHeight;

			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwSetWindowMonitor((GLFWwindow*)mWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			mData.mIsFullScreen = true;
		}
	}

	/*!***********************************************************************
		\brief
			Initializes the window
		\param[in] properties
			Properties of the window
	*************************************************************************/
	void WindowManager::Init(const WindowProperties& properties)
	{
		PROFILE_FUNCTION();
		mData.mWidth = properties.mWidth;
		mData.mHeight = properties.mHeight;
		mData.mTitle = properties.mTitle.c_str();
		mData.mIsFullScreen = false;

		BOREALIS_CORE_INFO("Creating window {0} with width ({1} and height {2})", mData.mTitle, mData.mWidth, mData.mHeight);
		
		FMOD_File_SetDiskBusy(1);

		if (!sGLFWInitialized)
		{
			PROFILE_SCOPE("glfwInit");
			int success = glfwInit();

			BOREALIS_CORE_ASSERT(success, "Failed to initialize GLFW!");

			sGLFWInitialized = true;
		}
		

		{
			PROFILE_SCOPE("glfwCreateWindow");

			//lock the version of opengl to 4.1
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			mWindow = (void*)(glfwCreateWindow((int)mData.mWidth, (int)mData.mHeight, mData.mTitle, nullptr, nullptr));
			mContext = new OpenGLContext((GLFWwindow*)mWindow);
			mContext->Init();
		}

		glfwSetWindowUserPointer((GLFWwindow*)mWindow, &mData);
		SetVSync(properties.mIsVSync);

		// Set up GLFW callbacks

		glfwSetErrorCallback(GLFWErrorCallback);

		glfwSetWindowSizeCallback((GLFWwindow*)mWindow, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.mWidth = width;
			data.mHeight = height;

			EventWindowResize event(width, height);
			data.mEventCallback(event);
		});

		glfwSetWindowCloseCallback((GLFWwindow*)mWindow, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			EventWindowClose event;
			data.mEventCallback(event);
		});

		glfwSetKeyCallback((GLFWwindow*)mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					EventKeyTriggered event(key);
					data.mEventCallback(event);
					EventKeyPressed event2(key);
					data.mEventCallback(event2);
					break;
				}
				case GLFW_RELEASE:
				{
					EventKeyReleased event(key);
					data.mEventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					EventKeyPressed event(key);
					data.mEventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback((GLFWwindow*)mWindow, [](GLFWwindow* window, unsigned  character)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			EventKeyTyped event(character);
			data.mEventCallback(event);
		});

		glfwSetMouseButtonCallback((GLFWwindow*)mWindow, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					EventMouseButtonTriggered event2(button);
					data.mEventCallback(event2);
					break;
				}
				case GLFW_RELEASE:
				{
					EventMouseButtonReleased event(button);
					data.mEventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback((GLFWwindow*)mWindow, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			EventMouseScrolled event(xOffset, yOffset);
			InputSystem::AccumulateScroll(yOffset);
			data.mEventCallback(event);
		});

		glfwSetCursorPosCallback((GLFWwindow*)mWindow, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			EventMouseMoved event(xPos, yPos);
			data.mEventCallback(event);
		});


	}

	/*!***********************************************************************
		\brief
			Shuts down the window
	*************************************************************************/
	void WindowManager::Shutdown()
	{
		PROFILE_FUNCTION();
		glfwDestroyWindow((GLFWwindow*)mWindow);
		delete mContext;
	}
} // End of namespace Borealis
