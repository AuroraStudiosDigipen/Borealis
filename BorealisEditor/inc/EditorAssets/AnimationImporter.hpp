#ifndef ANIMATIONIMPORTER_HPP
#define ANIMATIONIMPORTER_HPP

#include <Core/core.hpp>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "Graphics/Model.hpp"
#include "Graphics/Animation/Bone.hpp"
#include "Graphics/Animation/Animation.hpp"

namespace Borealis
{
	class AnimationImporter
	{
	public:
		static Ref<Animation> LoadAnimations(std::string const& animationPath, Ref<Model> model);

	private:
		static void ImportBones(aiNodeAnim* channel, std::vector<KeyPosition>& pos, std::vector<KeyRotation>& rot, std::vector<KeyScale>& scales);
		static void ReadMissingBones(aiAnimation const* animation, Ref<Model> model, Ref<Animation> anim);
		static void ReadHierarchyData(AssimpNodeData& dest, aiNode const* src);
	};
}

#endif