/******************************************************************************/
/*!
\file		Model.hpp
\author 	Chan Guo Geng Gordon
\par    	email: g.chan\@digipen.edu
\date   	September 12, 2024
\brief		Declares the class for Model for rendering

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef MODEL_HPP
#define MODEL_HPP
#include <vector>
#include <glm/glm.hpp>

#include <Assets/Asset.hpp>
#include <Core/Core.hpp>
#include <Graphics/Shader.hpp>
#include <Graphics/Mesh.hpp>

#include "Animation/Bone.hpp"

namespace Borealis
{
	struct MeshVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	class Model : public Asset
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

		/*!***********************************************************************
				TO REMOVE
		*************************************************************************/
		void SaveModel();

		std::vector<Mesh> mMeshes;
		std::map<std::string, BoneData> mBoneDataMap;
		int mBoneCounter;

	private:

		RTTR_ENABLE(Asset)

	}; // class Model
} // namespace Borealis
#endif