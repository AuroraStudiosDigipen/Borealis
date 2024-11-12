#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <filesystem>

#include <Core/Core.hpp>
#include <Assets/Asset.hpp>
#include <Assets/AssetMetaData.hpp>
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

	class Animation : public Asset
	{
	public:
		Animation() = default;

		Bone* FindBone(std::string const& name);

		void LoadAnimation(std::filesystem::path const& path);

		void Save();

		float mDuration;
		float mTicksPerSecond;
		std::vector<Bone> mBones;
		AssimpNodeData mRootNode;
		std::map<std::string, BoneData> mBoneDataMap;

		static Ref<Asset> Load(AssetMetaData const& assetMetaData);

	private:

		RTTR_ENABLE(Asset)
	};
}

#endif
