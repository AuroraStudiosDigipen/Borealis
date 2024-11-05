/******************************************************************************
/*!
\file       AnimationImporter.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       October 04, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <fstream>

#include "Importer/AnimationImporter.hpp"

namespace
{
	glm::mat4 ConvertMatrixtoGLM(aiMatrix4x4 const& mat)
	{
		glm::mat4 glm_mat;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		glm_mat[0][0] = mat.a1; glm_mat[1][0] = mat.a2; glm_mat[2][0] = mat.a3; glm_mat[3][0] = mat.a4;
		glm_mat[0][1] = mat.b1; glm_mat[1][1] = mat.b2; glm_mat[2][1] = mat.b3; glm_mat[3][1] = mat.b4;
		glm_mat[0][2] = mat.c1; glm_mat[1][2] = mat.c2; glm_mat[2][2] = mat.c3; glm_mat[3][2] = mat.c4;
		glm_mat[0][3] = mat.d1; glm_mat[1][3] = mat.d2; glm_mat[2][3] = mat.d3; glm_mat[3][3] = mat.d4;
		return glm_mat;
	}

	glm::vec3 GetGLMVec(const aiVector3D& vec)
	{
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	glm::quat GetGLMQuat(const aiQuaternion& pOrientation)
	{
		return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
	}

}

namespace BorealisAssetCompiler
{
	void ImportBones(aiNodeAnim* channel, std::vector<KeyPosition>& pos, std::vector<KeyRotation>& rot, std::vector<KeyScale>& scales)
	{
		unsigned numPositions = channel->mNumPositionKeys; //channel key pos

		for (unsigned positionIndex = 0; positionIndex < numPositions; ++positionIndex)
		{
			aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
			float timeStamp = (float)channel->mPositionKeys[positionIndex].mTime;
			KeyPosition data;
			data.position = GetGLMVec(aiPosition);
			data.timeStamp = timeStamp;
			pos.push_back(data);
		}

		unsigned numRotations = channel->mNumRotationKeys;
		for (unsigned rotationIndex = 0; rotationIndex < numRotations; ++rotationIndex)
		{
			aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
			float timeStamp = (float)channel->mRotationKeys[rotationIndex].mTime;
			KeyRotation data;
			data.orientation = GetGLMQuat(aiOrientation);
			data.timeStamp = timeStamp;
			rot.push_back(data);
		}

		unsigned numScalings = channel->mNumScalingKeys;
		for (unsigned keyIndex = 0; keyIndex < numScalings; ++keyIndex)
		{
			aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
			float timeStamp = (float)channel->mScalingKeys[keyIndex].mTime;
			KeyScale data;
			data.scale = GetGLMVec(scale);
			data.timeStamp = timeStamp;
			scales.push_back(data);
		}
	}

	void ReadBones(aiAnimation const* animation, Animation& anim)
	{
		unsigned size = animation->mNumChannels;
		auto& boneDataMap = anim.mBoneDataMap;
		unsigned boneCounter = 0;

		for (unsigned i{}; i < size; ++i)
		{
			auto aiChannel = animation->mChannels[i];
			std::string boneName = aiChannel->mNodeName.data; //channel name
			boneDataMap[boneName].id = boneCounter;
			boneCounter++;

			std::vector<KeyPosition> positions;
			std::vector<KeyRotation> rotations;
			std::vector<KeyScale> scales;
			ImportBones(aiChannel, positions, rotations, scales);
			anim.mBones.push_back(Bone(boneName, boneDataMap[boneName].id,
				positions, rotations, scales));
		}
	}

	void ReadHierarchyData(AssimpNodeData& dest, aiNode const* src)
	{
		dest.name = src->mName.data;	
		dest.transformation = ConvertMatrixtoGLM(src->mTransformation);
		dest.childrenCount = src->mNumChildren;

		for (unsigned int i{}; i < src->mNumChildren; ++i)
		{
			AssimpNodeData newData;
			ReadHierarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}

	void AnimationImporter::LoadAnimations(Animation& anim, std::string const& animationPath)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);

		assert(scene && scene->mRootNode);

		auto animation = scene->mAnimations[0];

		anim.mDuration = (float)animation->mDuration;
		anim.mTicksPerSecond = (float)animation->mTicksPerSecond;
		ReadHierarchyData(anim.mRootNode, scene->mRootNode);
		ReadBones(animation, anim);
	}

	void SaveAssimpNodeData(std::ofstream& outFile, const AssimpNodeData& node) {
		// Write transformation matrix
		outFile.write(reinterpret_cast<const char*>(&node.transformation), sizeof(node.transformation));

		// Write name length and name
		uint32_t nameLength = static_cast<uint32_t>(node.name.size());
		outFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
		outFile.write(node.name.data(), nameLength);

		// Write children count
		outFile.write(reinterpret_cast<const char*>(&node.childrenCount), sizeof(node.childrenCount));

		// Recursively write each child node
		for (const AssimpNodeData& child : node.children) {
			SaveAssimpNodeData(outFile, child);
		}
	}

	void AnimationImporter::SaveAnimation(Animation const& anim, std::filesystem::path const& path)
	{
		std::ofstream outFile(path, std::ios::binary);

		if (!outFile.is_open())
		{
			//ERROR CHECK
			return;
		}

		//save mDuration
		outFile.write(reinterpret_cast<const char*>(&anim.mDuration), sizeof(anim.mDuration));

		//save mTicksPerSecond
		outFile.write(reinterpret_cast<const char*>(&anim.mTicksPerSecond), sizeof(anim.mTicksPerSecond));

		/*
			for(bone : mBones)
				save bone
		*/
		uint32_t boneCount = static_cast<uint32_t>(anim.mBones.size());
		outFile.write(reinterpret_cast<const char*>(&boneCount), sizeof(boneCount));

		for (Bone const& bone : anim.mBones)
		{
			uint32_t nameLength = static_cast<uint32_t>(bone.mName.size());
			outFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
			outFile.write(bone.mName.data(), nameLength);

			// Write bone ID
			outFile.write(reinterpret_cast<const char*>(&bone.mId), sizeof(bone.mId));

			// Write the number of keyframes
			outFile.write(reinterpret_cast<const char*>(&bone.mNumPositions), sizeof(bone.mNumPositions));
			outFile.write(reinterpret_cast<const char*>(&bone.mNumRotations), sizeof(bone.mNumRotations));
			outFile.write(reinterpret_cast<const char*>(&bone.mNumScalings), sizeof(bone.mNumScalings));

			// Write all position keyframes
			for (const KeyPosition& pos : bone.mPositions)
			{
				outFile.write(reinterpret_cast<const char*>(&pos.position), sizeof(pos.position));
				outFile.write(reinterpret_cast<const char*>(&pos.timeStamp), sizeof(pos.timeStamp));
			}

			// Write all rotation keyframes
			for (const KeyRotation& rot : bone.mRotations)
			{
				outFile.write(reinterpret_cast<const char*>(&rot.orientation), sizeof(rot.orientation));
				outFile.write(reinterpret_cast<const char*>(&rot.timeStamp), sizeof(rot.timeStamp));
			}

			// Write all scale keyframes
			for (const KeyScale& scale : bone.mScales)
			{
				outFile.write(reinterpret_cast<const char*>(&scale.scale), sizeof(scale.scale));
				outFile.write(reinterpret_cast<const char*>(&scale.timeStamp), sizeof(scale.timeStamp));
			}
		}

		/*
			save assimpnodedata
		*/
		SaveAssimpNodeData(outFile, anim.mRootNode);

		outFile.close();
	}
}

