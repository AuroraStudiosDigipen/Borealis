/******************************************************************************/
/*!
\file		ImGuiLayer.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	February 19, 2024
\brief		Declares the ImGui Layer class that will be used to render the ImGui
			elements on the screen.

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/

#ifndef IMGUI_LAYER_HPP
#define IMGUI_LAYER_HPP
#include <Core/Layer.hpp>
#include <Events/EventInput.hpp>
#include <Events/EventWindow.hpp>
namespace Borealis
{
	/*!***********************************************************************
		\brief
			Class derived off Layer for ImGui rendering.
	*************************************************************************/
	class BOREALIS_API ImGuiLayer : public Layer
	{
	public:
		/*!***********************************************************************
			\brief
				Constructor for ImGuiLayer.
		*************************************************************************/
		ImGuiLayer();

		/*!***********************************************************************
			\brief
				Destructor for ImGuiLayer.
		*************************************************************************/
		~ImGuiLayer();

		/*!***********************************************************************
			\brief
				Initializes the ImGuiLayer.
		*************************************************************************/
		void Init();

		/*!***********************************************************************
			\brief
				Frees the ImGuiLayer.
		*************************************************************************/
		void Free();

		/*!***********************************************************************
			\brief
				Begins the Frame
		*************************************************************************/
		void BeginFrame();

		/*!***********************************************************************
			\brief
				Ends the Frame
		*************************************************************************/
		void EndFrame();

		/*!***********************************************************************
			\brief
				Renders the ImGui elements.
		*************************************************************************/
		void ImGuiRender(float dt) override;

		/*!***********************************************************************
			\brief
				Handles the events for the ImGuiLayer.
		*************************************************************************/
		void EventFn(Event& e) override;

		/*!***********************************************************************
			\brief
				Set whether to block events
			\param[in] block
				Whether to block events.
		*************************************************************************/
		void SetBlockEvents(bool block) { mBlockEvents = block; }

		/*!***********************************************************************
			\brief
				Window Resize Event Function
			\param[in] width
				Width of the window.
			\param[in] height
				Height of the window.
		*************************************************************************/
		void OnWindowResize(float width, float height);

		/*!***********************************************************************
			\brief
				Set ImGui to Dark Theme Colours
		*************************************************************************/
		static void SetDarkThemeColours();

		/*!***********************************************************************
			\brief
				Set ImGui to Light Theme Colours
		*************************************************************************/
		static void SetLightThemeColours();

	private:
		bool mBlockEvents = true; /*!< Whether to block events. */
		float mTime; /*!< Time for the ImGuiLayer. */
	};
} // End of namespace Borealis

#endif