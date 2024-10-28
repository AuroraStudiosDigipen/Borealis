#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <Core/Core.hpp>
#include "Bone.hpp"

namespace Borealis
{
	struct AssimpNodeData
	{
		glm::mat4 transformation;
		std::string name;
		int childrenCount;
		std::vector<AssimpNodeData> children;
	};

	class Animation
	{
	public:
		Animation() = default;

		Ref<Bone> FindBone(std::string const& name);

		/*float GetTicksPerSecond() const { return mTicksPerSecond; }
		float GetDuration() const { return mDuration; }
		AssimpNodeData const& GetRootNode() { return mRootNode; }
		std::map<std::string, BoneData> const& GetBoneDataMap() { return mBoneDataMap; }*/

		float mDuration;
		int mTicksPerSecond;
		std::vector<Bone> mBones;
		AssimpNodeData mRootNode;
		std::map<std::string, BoneData> mBoneDataMap;

	private:
		
	};
}

#endif
