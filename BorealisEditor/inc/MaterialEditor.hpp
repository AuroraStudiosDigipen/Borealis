#pragma once
#ifndef MATERIALEDITOR_HPP
#define MATERIALEDITOR_HPP

#include <Core/core.hpp>
#include <Scene/Entity.hpp>

namespace Borealis {
	class MaterialEditor
	{
	public:
		/*!***********************************************************************
			\brief
				Renders the material editor when creating new materials
		*************************************************************************/
		static void RenderEditor();

		/*!***********************************************************************
			\brief
				Renders the material's properties
			\param[in] material
				The material's properties to display
		*************************************************************************/
		static void RenderProperties(Ref<Material> const& material);

		///*!***********************************************************************
		//	\brief
		//		Getter for the render boolean
		//	\return
		//		render boolean
		//*************************************************************************/
		//static bool GetRender() { return mRender; }

		/*!***********************************************************************
			\brief
				Setter for the render boolean
			\param[in] render
				Render editor if true
		*************************************************************************/
		static void SetMaterial(AssetHandle materialHandle);
		
	private:
		static AssetHandle mMaterialHandle;
		static Ref<Material> mMaterial;
	};
}

#endif