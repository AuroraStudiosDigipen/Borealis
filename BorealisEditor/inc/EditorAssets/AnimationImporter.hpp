#ifndef ANIMATIONIMPORTER_HPP
#define ANIMATIONIMPORTER_HPP

#include <Core/core.hpp>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "Graphics/SkinnedModel.hpp"
#include "Graphics/Animation/Bone.hpp"
#include "Graphics/Animation/Animation.hpp"

namespace Borealis
{
	class AnimationImporter
	{
	public:
		static Ref<Animation> LoadAnimations(std::string const& animationPath);
		static void FillMissingBone(Ref<SkinnedModel> model, Ref<Animation> anim);

	private:
		static void ImportBones(aiNodeAnim* channel, std::vector<KeyPosition>& pos, std::vector<KeyRotation>& rot, std::vector<KeyScale>& scales);
		static void ReadBones(aiAnimation const* animation, Ref<Animation> anim);

		static void ReadHierarchyData(AssimpNodeData& dest, aiNode const* src);
	};
}

#endif