/******************************************************************************
/*!
\file       MaterialInstance.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       November 5, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef MaterialInstance_HPP
#define MaterialInstance_HPP

#include <Graphics/Material.hpp>

namespace Borealis
{
	class MaterialInstance
	{
	public:
		
		Ref<Shader> GetShader();

		static Ref<MaterialInstance> CreateInstance(Ref<Material> material);
	private:
		Ref<Material> mMaterial;

		std::unordered_map<Material::TextureMaps, Ref<Texture2D>> mTextureMaps; //Texture maps
		std::unordered_map<Material::TextureMaps, glm::vec4> mTextureMapColor; //Texture map colors
		std::unordered_map<Material::TextureMaps, float> mTextureMapFloat; //Texture map floats
		std::unordered_map<Material::Props, float> mPropertiesFloat; //Properties
		std::unordered_map<Material::Props, glm::vec2> mPropertiesVec2; //Properties
	};
}

#endif
