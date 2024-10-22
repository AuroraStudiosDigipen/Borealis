/******************************************************************************/
/*!
\file		ApplicationManager.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	February 15, 2024
\brief		Declares the functions for Application Manager of the Borealis

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef APPLICATION_MANAGER_HPP
#define APPLICATION_MANAGER_HPP

#include <Core/Core.hpp>
#include <Core/WindowManager.hpp>
#include <Core/EventSystem.hpp>
#include <Core/LayerSystem.hpp>
#include <Events/EventWindow.hpp>
#include <ImGui/ImGuiLayer.hpp>

//TO REMOVE ONCE RUNTIME ASSETS MANAGER IS UP
#include <Assets/EditorAssetManager.hpp>

namespace Borealis {

	/*!***********************************************************************
		\brief
			This class manages the application and the game loop
	*************************************************************************/
	class BOREALIS_API ApplicationManager
	{
	public:

		/*!***********************************************************************
			\brief
				Constructor of the class. Initializes the application
		*************************************************************************/
		ApplicationManager(const std::string& name = "Borealis");

		/*!***********************************************************************
			\brief
				Destructor of the class. Cleans up the application. 
		*************************************************************************/
		virtual ~ApplicationManager();

		/*!***********************************************************************
			\brief
				Run loop of the application.
		*************************************************************************/
		void Run();

		/*!***********************************************************************
			\brief
				Closes the application.
		*************************************************************************/
		void Close();

		/*!***********************************************************************
			\brief
				Runs an event
			\param e
				The event to run
		*************************************************************************/
		void EventFn(Event& e);

		/*!***********************************************************************
			\brief
				Pushs a layer into the layer system
			\param[in] layer
				The adderss of the layer to push
		*************************************************************************/
		void PushLayer(Layer* layer);

		/*!***********************************************************************
			\brief
				Pushs an overlay into the layer system
			\param[in] layer
				The adderss of the overlay to push
		*************************************************************************/
		void PushOverlay(Layer* overlay);

		/*!***********************************************************************
			\brief
				Gets the singleton class instance for the application manager
			\return
				The instance of the application manager
		*************************************************************************/
		inline static ApplicationManager& Get() { return *sInstance; } //<! Returns the instance of the application manager

		/*!***********************************************************************
			\brief
				Gets the window manager
			\return
				The window manager
		*************************************************************************/
		inline WindowManager* GetWindow() const { return mWindowManager; }

		/*!***********************************************************************
			\brief
				Gets the ImGui layer
			\return
				The ImGui layer
		*************************************************************************/
		ImGuiLayer* GetImGuiLayer() const { return mImGuiLayer; }

	private:

		/*!***********************************************************************
			\brief
				Handles the window close event
			\param e
				The window close event
			\return
				True if the event is handled
		*************************************************************************/
		bool WindowCloseHandler(EventWindowClose& e);

		/*!***********************************************************************
			\brief
				Handles the window resize event
			\param e
				The window resize event
			\return
				True if the event is handled
		*************************************************************************/
		bool WindowResizeHandler(EventWindowResize& e);
		
		LayerSystem mLayerSystem; //<! Layer system of the application
		WindowManager* mWindowManager; //<! Pointer to the window manager
		ImGuiLayer* mImGuiLayer; //<! Pointer to the ImGui layer
		bool mIsRunning; //<! Flag to check if the application is running
		bool mIsMinimized; //<! Flag to check if the application is minimized

		static ApplicationManager* sInstance; //<! Pointer to the instance of the application manager
	}; // End of class ApplicationManager

	/*!***********************************************************************
		\brief
			This is a factory function that creates an instance of the
			ApplicationManager. It is to be defined by the client after creating 
			their own derived application by inheriting from the Application 
			Manager class.
		\return
			Pointer to the ApplicationManager
	*************************************************************************/
	ApplicationManager* CreateApplication();
} // End of namespace Borealis

#endif