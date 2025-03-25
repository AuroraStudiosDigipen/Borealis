#include <BorealisPCH.hpp>
#include <Core/LoggerSystem.hpp>
#include "Graphics/Animation/Animation.hpp"

namespace Borealis
{
	Bone* Animation::FindBone(std::string const& name)
	{
		auto i = std::find_if(mBones.begin(), mBones.end(), [&](Bone const& bone)
			{
				return bone.GetBoneName() == name;
			});

		if (i == mBones.end())
		{
			return nullptr;
		}

		return &(*i);
	}

	void LoadAssimpNodeData(std::ifstream& inFile, AssimpNodeData& node) 
	{
		// Read transformation matrix
		inFile.read(reinterpret_cast<char*>(&node.transformation), sizeof(node.transformation));

		// Read name length and name
		uint32_t nameLength;
		inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
		node.name.resize(nameLength);
		inFile.read(&node.name[0], nameLength);

		// Read children count
		inFile.read(reinterpret_cast<char*>(&node.childrenCount), sizeof(node.childrenCount));

		// Recursively read each child node
		node.children.resize(node.childrenCount);
		for (AssimpNodeData& child : node.children) {
			LoadAssimpNodeData(inFile, child);
		}
	}

	void Animation::LoadAnimation(std::filesystem::path const& path)
	{
		std::ifstream inFile(path, std::ios::binary);
		if (!inFile.is_open()) 
		{
			BOREALIS_CORE_ASSERT(!inFile.is_open(), "File Not found");
			return;
		}

		// Load mDuration
		inFile.read(reinterpret_cast<char*>(&mDuration), sizeof(mDuration));

		// Load mTicksPerSecond
		inFile.read(reinterpret_cast<char*>(&mTicksPerSecond), sizeof(mTicksPerSecond));

		// Load the number of bones
		uint32_t boneCount;
		inFile.read(reinterpret_cast<char*>(&boneCount), sizeof(boneCount));
		mBones.resize(boneCount);

		for (Bone& bone : mBones) 
		{
			// Load the length of the name and then the name itself
			uint32_t nameLength;
			inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
			bone.mName.resize(nameLength);
			inFile.read(&bone.mName[0], nameLength);

			// Load bone ID
			inFile.read(reinterpret_cast<char*>(&bone.mId), sizeof(bone.mId));

			// Load the number of keyframes for positions, rotations, and scales
			inFile.read(reinterpret_cast<char*>(&bone.mNumPositions), sizeof(bone.mNumPositions));
			inFile.read(reinterpret_cast<char*>(&bone.mNumRotations), sizeof(bone.mNumRotations));
			inFile.read(reinterpret_cast<char*>(&bone.mNumScalings), sizeof(bone.mNumScalings));

			// Load all position keyframes
			bone.mPositions.resize(bone.mNumPositions);
			for (KeyPosition& pos : bone.mPositions) 
			{
				inFile.read(reinterpret_cast<char*>(&pos.position), sizeof(pos.position));
				inFile.read(reinterpret_cast<char*>(&pos.timeStamp), sizeof(pos.timeStamp));
			}

			// Load all rotation keyframes
			bone.mRotations.resize(bone.mNumRotations);
			for (KeyRotation& rot : bone.mRotations) 
			{
				inFile.read(reinterpret_cast<char*>(&rot.orientation), sizeof(rot.orientation));
				inFile.read(reinterpret_cast<char*>(&rot.timeStamp), sizeof(rot.timeStamp));
			}

			// Load all scale keyframes
			bone.mScales.resize(bone.mNumScalings);
			for (KeyScale& scale : bone.mScales) 
			{
				inFile.read(reinterpret_cast<char*>(&scale.scale), sizeof(scale.scale));
				inFile.read(reinterpret_cast<char*>(&scale.timeStamp), sizeof(scale.timeStamp));
			}
		}

		// Load AssimpNodeData structure (root node of the hierarchy)
		LoadAssimpNodeData(inFile, mRootNode);

		inFile.close();
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

	void Animation::Save()
	{
		std::ofstream outFile("anim.anim", std::ios::binary);

		if (!outFile.is_open()) 
		{
			//ERROR CHECK
			return;
		}

		//save mDuration
		outFile.write(reinterpret_cast<const char*>(&mDuration), sizeof(mDuration));

		//save mTicksPerSecond
		outFile.write(reinterpret_cast<const char*>(&mTicksPerSecond), sizeof(mTicksPerSecond));

		/*
			for(bone : mBones)
				save bone
		*/
		uint32_t boneCount = static_cast<uint32_t>(mBones.size());
		outFile.write(reinterpret_cast<const char*>(&boneCount), sizeof(boneCount));

		for (Bone const& bone : mBones)
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
		SaveAssimpNodeData(outFile, mRootNode);

		outFile.close();
	}
	Ref<Asset> Animation::Load(std::filesystem::path const& cachePath,AssetMetaData const& assetMetaData)
	{
		Animation anim;
		anim.LoadAnimation(cachePath/std::to_string(assetMetaData.Handle));
		return MakeRef<Animation>(anim);
	}
	void Animation::Reload(AssetMetaData const& assetMetaData, Ref<Asset> const& asset)
	{
		Animation anim;
		anim.LoadAnimation(assetMetaData.CachePath);
		Ref<Animation> newAsset = MakeRef<Animation>(anim);
		newAsset->mAssetHandle = assetMetaData.Handle;

		newAsset->swap(*asset);
	}
	void Animation::swap(Asset& other)
	{
		Animation& o = static_cast<Animation&>(other);
		std::swap(mDuration, o.mDuration);
		std::swap(mTicksPerSecond, o.mTicksPerSecond);
		std::swap(mBones, o.mBones);
		std::swap(mRootNode, o.mRootNode);
		std::swap(mBoneDataMap, o.mBoneDataMap);
	}
}