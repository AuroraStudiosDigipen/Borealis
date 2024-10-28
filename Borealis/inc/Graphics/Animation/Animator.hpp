#ifndef ANIMATOR_HPP
#define ANIMATOR_HPP

#include <Core/Core.hpp>
#include "Graphics/Animation/Animation.hpp"

namespace Borealis
{
	class Animator
	{
	public:
		Animator(Ref<Animation> animation);

		void UpdateAnimation(float dt);
		void PlayAnimation(Ref<Animation> animation);
		void CalculateBoneTransform(AssimpNodeData const* node, glm::mat4 parentTransform);

		std::vector<glm::mat4> GetFinalBoneMatrices() { return mFinalBoneMatrices; }

	private:
		std::vector<glm::mat4> mFinalBoneMatrices;
		Ref<Animation> mCurrentAnimation;
		float mCurrentTime;
		float mDeltaTime;
	};
}

#endif