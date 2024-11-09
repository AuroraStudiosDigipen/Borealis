/******************************************************************************/
/*!
\file		ApplicationManager.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	February 15, 2024
\brief		Defines the functions for Application Manager of the Borealis

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#define NOMINMAX
#include <windows.h>
#include <GLFW/glfw3.h>
#include <Audio/AudioEngine.hpp>
#include <Core/LoggerSystem.hpp>
#include <Core/ApplicationManager.hpp>
#include <Core/InputSystem.hpp>
#include <Graphics/Renderer.hpp>
#include <Scripting/ScriptingSystem.hpp>
#include <AI/BehaviourTree/RegisterNodes.hpp>
#include <Physics/PhysicsSystem.hpp>
#include <Core/TimeManager.hpp>

namespace Borealis
{
	ApplicationManager* ApplicationManager::sInstance = nullptr;

	/*!***********************************************************************
		\brief
			Constructor of the class. Initializes the application
	*************************************************************************/
	ApplicationManager::ApplicationManager(const std::string& name)
	{
		PROFILE_FUNCTION();
		mIsRunning = true;
		mIsMinimized = false;
		sInstance = this;
		mWindowManager = new WindowManager(WindowProperties(name));
		mWindowManager->SetEventCallback(BIND_EVENT(ApplicationManager::EventFn));

		mImGuiLayer = new ImGuiLayer();
		PushOverlay(mImGuiLayer);

		Renderer::Init();

		ScriptingSystem::Init();

		AudioEngine::Init();
		NodeFactory::Init();
		InputSystem::Init();
		PhysicsSystem::Init();
	}

	/*!***********************************************************************
		\brief
			Destructor of the class. Cleans up the application.
	*************************************************************************/
	ApplicationManager::~ApplicationManager()
	{
		PROFILE_FUNCTION();

		Renderer::Free();
		AudioEngine::Shutdown();
		mLayerSystem.Clear();
		ScriptingSystem::Free();

		PhysicsSystem::Free();
		delete mWindowManager;
		glfwTerminate(); // Terminate after system shuts down

	}

	/*!***********************************************************************
		\brief
			Run loop of the application.
	*************************************************************************/
	void ApplicationManager::Run()
	{
		ULONGLONG prevTickCount = GetTickCount64(); // Get initial tick count
		PROFILE_FUNCTION();
		while (mIsRunning)
		{
			PROFILE_SCOPE("ApplicationManager Run Loop");

			ULONGLONG currentTickCount = GetTickCount64();
			ULONGLONG deltaTime = currentTickCount - prevTickCount;
			prevTickCount = currentTickCount;

			TimeManager::SetDeltaTime(static_cast<float>(deltaTime) / 1000.0f);

			if (!mIsMinimized)
			{
				InputSystem::PollInput();
				{
					PROFILE_SCOPE("LayerStack Updates");
					for (Layer* layer : mLayerSystem)
						layer->UpdateFn(TimeManager::GetDeltaTime());
				}


				mImGuiLayer->BeginFrame();
				{
					PROFILE_SCOPE("LayerStack ImGuiRender");
					for (Layer* layer : mLayerSystem)
						layer->ImGuiRender(TimeManager::GetDeltaTime());
				}
				mImGuiLayer->EndFrame();
			}

			InputSystem::ResetScroll();
			mWindowManager->OnUpdate();
		}
		
	}
	void ApplicationManager::Close()
	{
		mIsRunning = false;
	}
	/*!***********************************************************************
		\brief
			Runs an event
		\param e
			The event to run
	*************************************************************************/
	void ApplicationManager::EventFn(Event& e)
	{
		PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<EventWindowClose>(BIND_EVENT(ApplicationManager::WindowCloseHandler));
		dispatcher.Dispatch<EventWindowResize>(BIND_EVENT(ApplicationManager::WindowResizeHandler));

		for (auto it = mLayerSystem.end(); it != mLayerSystem.begin();)
		{
			(*--it)->EventFn(e);
			if (e.isDoneHandling)
				break;
		}
	}

	/*!***********************************************************************
		\brief
			Pushs a layer into the layer system
		\param[in] layer
			The adderss of the layer to push
	*************************************************************************/
	void ApplicationManager::PushLayer(Layer* layer)
	{
		PROFILE_FUNCTION();

		mLayerSystem.PushLayer(layer);
		layer->Init();
	}

	/*!***********************************************************************
		\brief
			Pushs an overlay into the layer system
		\param[in] layer
			The adderss of the overlay to push
	*************************************************************************/
	void ApplicationManager::PushOverlay(Layer* overlay)
	{
		PROFILE_FUNCTION();

		mLayerSystem.PushOverlay(overlay);
		overlay->Init();
	}

	/*!***********************************************************************
		\brief
			Handles the window close event
		\param e
			The window close event
		\return
			True if the event is handled
	*************************************************************************/
	bool ApplicationManager::WindowCloseHandler(EventWindowClose& e)
	{
		mIsRunning = false;
		return true;
	}
	bool ApplicationManager::WindowResizeHandler(EventWindowResize& e)
	{
		PROFILE_FUNCTION();
		if(e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			mIsMinimized = true;
			return false;
		}

		mIsMinimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		mImGuiLayer->OnWindowResize(static_cast<float>(e.GetWidth()), static_cast<float>(e.GetHeight()));
		return false;
	}
}
