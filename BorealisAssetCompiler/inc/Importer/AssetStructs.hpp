/******************************************************************************
/*!
\file       AssetStructs.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       October 23, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef AssetStructs_HPP
#define AssetStructs_HPP

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <map>

namespace BorealisAssetCompiler
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	struct Mesh
	{
		std::vector<unsigned int> mIndices;
		std::vector<Vertex> mVertices;

		uint32_t mVerticesCount;
		uint32_t mIndicesCount;
	};

	struct Model
	{
	public:
		std::vector<Mesh> mMeshes;
	};

	//============================================================================================
	// Skinned Mesh
	//============================================================================================
#define MAX_BONE_INFLUENCE 4

	struct VertexBoneData
	{
		int32_t mBoneIds[MAX_BONE_INFLUENCE] = {};
		float mWeights[MAX_BONE_INFLUENCE] = {};

		VertexBoneData() {}

		void AddBoneData(int32_t boneid, float weight) {

			for (int32_t i{}; i < MAX_BONE_INFLUENCE; ++i)
			{
				if (mBoneIds[i] < 0)
				{
					mBoneIds[i] = boneid;
					mWeights[i] = weight;
					//printf("bone %d weight %f index %i\n", boneid, weight, i);
					break;
				}
			}
		}

		void ResetBoneData()
		{
			for (int i{}; i < MAX_BONE_INFLUENCE; ++i)
			{
				mBoneIds[i] = -1;
				mWeights[i] = 0.f;
			}
		}
	};

	struct SkinnedVertex 
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		VertexBoneData BoneData;
	};

	class SkinnedMesh
	{
	public:
		std::vector<unsigned int> mIndices;
		std::vector<SkinnedVertex> mVertices;

		uint32_t mVerticesCount; // Number of vertices
		uint32_t mIndicesCount; // Number of indices

		SkinnedMesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<VertexBoneData>& boneData)
		{
			mIndices = indices;
			mIndicesCount = (uint32_t)indices.size();
			mVerticesCount = (uint32_t)vertices.size();
			//mNormals = normals;

			for (int i{}; i < vertices.size(); i++)
			{
				SkinnedVertex vertex;
				vertex.Position = vertices[i];
				vertex.Normal = normals[i];
				vertex.TexCoords = texCoords[i];
				vertex.BoneData = boneData[i];

				mVertices.push_back(vertex);
			}
		}
	};

	struct BoneData
	{
		//std::string name;
		int id;
		glm::mat4 offsetMatrix; // xform matrix for the bone
	};

	struct KeyPosition
	{
		glm::vec3 position;
		float timeStamp;
	};

	struct KeyRotation
	{
		glm::quat orientation;
		float timeStamp;
	};

	struct KeyScale
	{
		glm::vec3 scale;
		float timeStamp;
	};

	class Bone
	{
	public:
		Bone() = default;
		Bone(std::string const& name, int id, std::vector<KeyPosition> const& pos,
			std::vector<KeyRotation> const& rot,
			std::vector<KeyScale> const& scale);

		void Update(float animationTime);

		glm::mat4 GetLocalTransform() { return mLocalTransform; }
		std::string GetBoneName() const { return mName; }
		int GetBoneID() { return mId; }
		int GetPositionIndex(float animationTime);
		int GetRotationIndex(float animationTime);
		int GetScaleIndex(float animationTime);

		int mNumPositions;
		int mNumRotations;
		int mNumScalings;

		std::string mName;
		int mId;

		std::vector<KeyPosition> mPositions;
		std::vector<KeyRotation> mRotations;
		std::vector<KeyScale> mScales;

		glm::mat4 mLocalTransform;
	private:
		float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

		glm::mat4 InterpolatePosition(float animationTime);
		glm::mat4 InterpolateRotation(float animationTime);
		glm::mat4 InterpolateScaling(float animationTime);

	};

	class SkinnedModel
	{
	public:
		std::vector<SkinnedMesh> mMeshes;
		std::map<std::string, BoneData> mBoneDataMap{};
		int mBoneCounter{};
	};
}

#endif
