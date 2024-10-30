#include "EditorAssets/AnimationImporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace Borealis
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

	Ref<Animation> AnimationImporter::LoadAnimations(std::string const& animationPath, Ref<Model> model)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
		
		auto animation = scene->mAnimations[0];
		Ref<Animation> anim = MakeRef<Animation>();

		anim->mDuration = animation->mDuration;
		anim->mTicksPerSecond = animation->mTicksPerSecond;
		aiMatrix4x4 globalXform = scene->mRootNode->mTransformation;
		globalXform = globalXform.Inverse();

		ReadHierarchyData(anim->mRootNode, scene->mRootNode);
		ReadMissingBones(animation, model, anim);

		return anim;
	}

	void AnimationImporter::ImportBones(aiNodeAnim* channel, std::vector<KeyPosition>& pos, std::vector<KeyRotation>& rot, std::vector<KeyScale>& scales)
	{
		unsigned numPositions = channel->mNumPositionKeys;

		for (int positionIndex = 0; positionIndex < numPositions; ++positionIndex)
		{
			aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
			float timeStamp = channel->mPositionKeys[positionIndex].mTime;
			KeyPosition data;
			data.position = GetGLMVec(aiPosition);
			data.timeStamp = timeStamp;
			pos.push_back(data);
		}

		unsigned numRotations = channel->mNumRotationKeys;
		for (int rotationIndex = 0; rotationIndex < numRotations; ++rotationIndex)
		{
			aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
			float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
			KeyRotation data;
			data.orientation = GetGLMQuat(aiOrientation);
			data.timeStamp = timeStamp;
			rot.push_back(data);
		}

		unsigned numScalings = channel->mNumScalingKeys;
		for (int keyIndex = 0; keyIndex < numScalings; ++keyIndex)
		{
			aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
			float timeStamp = channel->mScalingKeys[keyIndex].mTime;
			KeyScale data;
			data.scale = GetGLMVec(scale);
			data.timeStamp = timeStamp;
			scales.push_back(data);
		}
	}

	void AnimationImporter::ReadMissingBones(aiAnimation const* animation, Ref<Model> model, Ref<Animation> anim)
	{
		int size = animation->mNumChannels;
		auto& boneDataMap = model->mBoneDataMap;
		int& boneCount = model->mBoneCounter;

		for (int i{}; i < size; ++i)
		{
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;

			if (boneDataMap.find(boneName) == boneDataMap.end())
			{
				boneDataMap[boneName].id = boneCount;
				++boneCount;
			}

			std::vector<KeyPosition> positions;
			std::vector<KeyRotation> rotations;
			std::vector<KeyScale> scales;
			ImportBones(channel, positions, rotations, scales);
			anim->mBones.push_back(Bone(channel->mNodeName.data, boneDataMap[channel->mNodeName.data].id,
				positions, rotations, scales));
		}

		anim->mBoneDataMap = boneDataMap;
	}

	void AnimationImporter::ReadHierarchyData(AssimpNodeData& dest, aiNode const* src)
	{
		dest.name = src->mName.data;
		dest.transformation = ConvertMatrixtoGLM(src->mTransformation);
		dest.childrenCount = src->mNumChildren;

		for (int i{}; i < src->mNumChildren; ++i)
		{
			AssimpNodeData newData;
			ReadHierarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}
}
