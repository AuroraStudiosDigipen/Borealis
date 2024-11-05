/******************************************************************************
/*!
\file       SkinnedModel.hpp
\author 	Vanesius Faith Cheong
\par    	email: vanesiusfaith.c\@digipen.edu
\date       October 30, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef SkinnedModel_HPP
#define SkinnedModel_HPP

#include <Assets/Asset.hpp>
#include <Core/Core.hpp>
#include <Graphics/Shader.hpp>
#include <Graphics/SkinnedMesh.hpp>

#include <Graphics/Animation/Animation.hpp>


namespace Borealis
{
	class SkinnedModel : public Asset
	{
	public:
		/*!***********************************************************************
			\brief
				Renders the model
		*************************************************************************/
		void Draw(const glm::mat4& transform, Ref<Shader> shader, int entityID);

		/*!***********************************************************************
			\brief
				Load the model from a file path
		*************************************************************************/
		void LoadModel(std::filesystem::path const& path);

		void AssignAnimation(Ref<Animation> animation);

		/*!***********************************************************************
				TO REMOVE
		*************************************************************************/
		void SaveModel();

		std::vector<SkinnedMesh> mMeshes;
		std::map<std::string, BoneData> mBoneDataMap{};
		int mBoneCounter{};
		Ref<Animation> mAnimation;
	private:
		RTTR_ENABLE(Asset)
	};
}

#endif
