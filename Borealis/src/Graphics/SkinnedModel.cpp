/******************************************************************************
/*!
\file       SkinnedModel.cpp
\author 	Vanesius Faith Cheong
\par    	email: vanesiusfaith.c\@digipen.edu
\date       October 30, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>

#include <Graphics/SkinnedModel.hpp>

namespace Borealis
{
	void SkinnedModel::Draw(const glm::mat4& transform, Ref<Shader> shader, int entityID)
	{
		for (auto mesh : mMeshes)
		{
			mesh.Draw(transform, shader, entityID);
		}
	}

	void SkinnedModel::LoadModel(std::filesystem::path const& path)
	{
		std::ifstream inFile(path, std::ios::binary);

		uint32_t meshCount;
		inFile.read(reinterpret_cast<char*>(&meshCount), sizeof(meshCount));

		mMeshes.resize(meshCount);

		for (SkinnedMesh& mesh : mMeshes)
		{
			//always the same?
			uint32_t verticesCount, indicesCount;
			inFile.read(reinterpret_cast<char*>(&verticesCount), sizeof(verticesCount));
			mesh.SetVerticesCount(verticesCount);
			inFile.read(reinterpret_cast<char*>(&indicesCount), sizeof(indicesCount));
			mesh.SetIndicesCount(indicesCount);

			mesh.GetVertices().resize(verticesCount);
			mesh.GetIndices().resize(indicesCount);

			inFile.read(reinterpret_cast<char*>(mesh.GetVertices().data()), verticesCount * sizeof(SkinnedVertex));

			inFile.read(reinterpret_cast<char*>(mesh.GetIndices().data()), indicesCount * sizeof(uint32_t));

			mesh.SetupMesh();
		}

		// Load bone data map
		uint32_t boneDataMapSize;
		inFile.read(reinterpret_cast<char*>(&boneDataMapSize), sizeof(boneDataMapSize));

		for (uint32_t i = 0; i < boneDataMapSize; ++i) {
			// Read name length
			uint32_t nameLength;
			inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));

			// Read the bone name
			std::string name(nameLength, '\0');
			inFile.read(&name[0], nameLength);

			// Read the BoneData
			BoneData boneData;
			inFile.read(reinterpret_cast<char*>(&boneData.id), sizeof(boneData.id));
			inFile.read(reinterpret_cast<char*>(&boneData.offsetMatrix), sizeof(boneData.offsetMatrix));

			// Add to the map
			mBoneDataMap[name] = boneData;
		}

		// Load bone counter
		inFile.read(reinterpret_cast<char*>(&mBoneCounter), sizeof(mBoneCounter));

		inFile.close();
	}

	void SkinnedModel::AssignAnimation(Ref<Animation> animation)
	{
		for (int i{}; i < animation->mBones.size(); ++i)
		{
			Bone& bone = animation->mBones[i];
			std::string boneName = bone.GetBoneName();

			if (mBoneDataMap.find(boneName) == mBoneDataMap.end())
			{
				mBoneDataMap[boneName].id = bone.GetBoneID();
			}
		}

		animation->mBoneDataMap = mBoneDataMap;

		mAnimation = animation;
	}

	void SkinnedModel::SaveModel()
	{
		std::ofstream outFile("model.skmesh", std::ios::binary);

		uint32_t meshCount = static_cast<uint32_t>(mMeshes.size());
		outFile.write(reinterpret_cast<const char*>(&meshCount), sizeof(meshCount));

		for (const SkinnedMesh& mesh : mMeshes) {
			// Write mesh header (vertexCount, indexCount, normalCount, texCoordCount)
			uint32_t verticesCount = mesh.GetVerticesCount();
			uint32_t indicesCount = mesh.GetIndicesCount();
			outFile.write(reinterpret_cast<const char*>(&verticesCount), sizeof(verticesCount));
			outFile.write(reinterpret_cast<const char*>(&indicesCount), sizeof(indicesCount));

			// Write vertices
			outFile.write(reinterpret_cast<const char*>(mesh.GetVertices().data()), verticesCount * sizeof(SkinnedVertex));

			// Write indices
			outFile.write(reinterpret_cast<const char*>(mesh.GetIndices().data()), indicesCount * sizeof(uint32_t));
		}

		//save map of bonedata
		uint32_t boneDataMapSize = static_cast<uint32_t>(mBoneDataMap.size());
		outFile.write(reinterpret_cast<const char*>(&boneDataMapSize), sizeof(boneDataMapSize));

		for (const auto& [name, boneData] : mBoneDataMap) {
			// Write the length of the string key
			uint32_t nameLength = static_cast<uint32_t>(name.size());
			outFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));

			// Write the string key
			outFile.write(name.data(), nameLength);

			// Write the BoneData
			outFile.write(reinterpret_cast<const char*>(&boneData.id), sizeof(boneData.id));
			outFile.write(reinterpret_cast<const char*>(&boneData.offsetMatrix), sizeof(boneData.offsetMatrix));
		}

		//save bone counter
		outFile.write(reinterpret_cast<const char*>(&mBoneCounter), sizeof(mBoneCounter));

		outFile.close();
	}
}
