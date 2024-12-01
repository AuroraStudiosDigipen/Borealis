/******************************************************************************/
/*!
\file		WindowManager.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	February 17, 2024
\brief		Declares the functions for Window Manager of the Borealis

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/

#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#include <Core/Core.hpp>
#include <Core/EventSystem.hpp>
#include <Graphics/GraphicsContext.hpp>
namespace Borealis
{
	/*!***********************************************************************
		\brief
			Struct to store the properties of the window
	*************************************************************************/
	struct WindowProperties
	{
		std::string mTitle;
		uint32_t mWidth;
		uint32_t mHeight;
		bool mIsVSync;

		/*!***********************************************************************
			\brief
				Constructor for WindowProperties
			\param[in] title
				Title of the window
			\param[in] width
				Width of the window
			\param[in] height
				Height of the window
			\param[in] isVSync
				Whether VSync is enabled
		*************************************************************************/
		WindowProperties(std::string title = "Borealis", uint32_t width = 1920, uint32_t height = 1080, bool isVSync = true)
			: mTitle(title), mWidth(width), mHeight(height), mIsVSync(isVSync) {}

	}; // End of struct WindowProperties

	/*!***********************************************************************
		\brief
			Class to manage the window
	*************************************************************************/
	class BOREALIS_API WindowManager
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		/*!***********************************************************************
			\brief
				Constructor for WindowManager
			\param[in] properties
				Properties of the window
		*************************************************************************/
		WindowManager(const WindowProperties& properties = WindowProperties());

		/*!***********************************************************************
			\brief
				Destructor for WindowManager
		*************************************************************************/
		virtual ~WindowManager();

		/*!***********************************************************************
			\brief
				Updates the window
		*************************************************************************/
		void OnUpdate();

		/*!***********************************************************************
			\brief
				Getter for the window width
			\return
				Window Width as an unsigned int
		*************************************************************************/
		inline unsigned int GetWidth() const { return mData.mWidth; }

		/*!***********************************************************************
			\brief
				Getter for the window height
			\return
				Window Height as an unsigned int
		*************************************************************************/
		inline unsigned int GetHeight() const { return mData.mHeight; }

		/*!***********************************************************************
			\brief
				Setter for event callback function
			\param[in] callback
				Function to be called when an event occurs
		*************************************************************************/
		inline void SetEventCallback(const EventCallbackFn& callback) { mData.mEventCallback = callback; }

		/*!***********************************************************************
			\brief
				Setter for the Window VSync
			\param[in] enabled
				Whether VSync is enabled
		*************************************************************************/
		void SetVSync(bool enabled);

		/*!***********************************************************************
			\brief
				Getter for the Window VSync
			\return
				True if VSync is enabled, false otherwise
		*************************************************************************/
		bool IsVSync() const;

		/*!***********************************************************************
			\brief
				Getter for the native window
			\return
				Pointer to the native window
		*************************************************************************/
		void* GetNativeWindow() const { return mWindow; }

		bool GetCursorVisibility();

		void SetCursorVisibility(bool visible);

		void ToggleFullScreen();

	private:
		/*!***********************************************************************
			\brief
				Initializes the window
			\param[in] properties
				Properties of the window
		*************************************************************************/
		virtual void Init(const WindowProperties& properties);

		/*!***********************************************************************
			\brief
				Shuts down the window
		*************************************************************************/
		virtual void Shutdown();

		void* mWindow; // Pointer to the native window
		GraphicsContext* mContext; // Pointer to the graphics context

		struct WindowData
		{
			const char* mTitle; // Title of the window
			unsigned mWidth;	// Width of the window
			unsigned mHeight;	// Height of the window
			unsigned mPosX;
			unsigned mPosY;
			bool mIsFullScreen;
			bool mIsVSync;		// Whether VSync is enabled

			EventCallbackFn mEventCallback; // Callback function for events
		};

		WindowData mData; // Data of the window
	}; // End of Class WindowManager

} // End of namespace Borealis

#endif // End of WINDOW_MANAGER_HPP