/******************************************************************************/
/*!
\file		Layer.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	February 18, 2024
\brief		Declares the Layer Class to be used by Layer System in Borealis

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/

#ifndef LAYER_HPP
#define LAYER_HPP

#include <Core/EventSystem.hpp>
#include <Core/Core.hpp>
namespace Borealis
{
	/*!***********************************************************************
		\brief
			Class to be inherited by all layers in the engine
	*************************************************************************/
	class BOREALIS_API Layer
	{
	public:
#ifndef _DIST
		/*!***********************************************************************
			\brief
				Constructor for Layer
			\param[in] name
				Name of the layer
			\note
				Layer name is used for debugging purposes, not available in
				distribution build
		*************************************************************************/
		Layer(const std::string& name = "Layer");
#else
		/*!***********************************************************************
			\brief
				Default Constructor for Layer
			\note
				Used in distribution build. Layer name is not available in
				non Distribution build.
		*************************************************************************/
		Layer();
#endif
		/*!***********************************************************************
			\brief
				Destructor for Layer
		*************************************************************************/
		virtual ~Layer();

		/*!***********************************************************************
			\brief
				Initializes the Layer
		*************************************************************************/
		virtual void Init() {}

		/*!***********************************************************************
			\brief
				Free the Layer
		*************************************************************************/
		virtual void Free() {}

		/*!***********************************************************************
			\brief
				Update function of the layer
			\param[in] dt
				Delta Time
		*************************************************************************/
		virtual void UpdateFn(float dt) {}

		/*!***********************************************************************
			\brief
				Event Function of the layer
			\param[in] e
				Event to be handled
		*************************************************************************/
		virtual void EventFn(Event& e) {}

		/*!***********************************************************************
			\brief
				ImGui Rendering function of the layer
		*************************************************************************/
		virtual void ImGuiRender(float dt) {};

#ifndef _DIST

		/*!***********************************************************************
			\brief
				Get the name of the layer
			\return
				Name of the layer
		*************************************************************************/
		const std::string& GetName() const { return *mName; }

	protected:
		std::string* mName; //!< Name of the layer
#endif
	}; // End of Layer Class


} // End of namespace Borealis

#endif