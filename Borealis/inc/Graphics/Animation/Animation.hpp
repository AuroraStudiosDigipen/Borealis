#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <filesystem>

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

		Bone* FindBone(std::string const& name);

		void Load(std::filesystem::path const& path);

		void Save();

		float mDuration;
		int mTicksPerSecond;
		std::vector<Bone> mBones;
		AssimpNodeData mRootNode;
		std::map<std::string, BoneData> mBoneDataMap;

	private:
		
	};
}

#endif
