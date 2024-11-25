#ifndef ANIMATOR_HPP
#define ANIMATOR_HPP

#include <Core/Core.hpp>

#include "Graphics/Model.hpp"
#include "Graphics/Animation/Animation.hpp"

namespace Borealis
{
	class Animator
	{
	public:
		Animator();
		Animator(Ref<Animation> animation);

		void UpdateAnimation(float dt);
		bool HasAnimation();
		void PlayAnimation(Ref<Animation> animation);
		void CalculateBoneTransform(AssimpNodeData const* node, glm::mat4 parentTransform);
		void CalculateBlendedBoneTransform(Ref<Animation> animationBase, AssimpNodeData const* node,
			Ref<Animation> animationLayer, AssimpNodeData const* nodeLayer,
			float currentTimeBase, float currentTimeLayer, glm::mat4 const& parentTransform, float blendFactor);
		void BlendTwoAnimations(Ref<Animation> baseAnimation, Ref<Animation> layerAnimation, float blendFactor, float deltaTime);

		std::vector<glm::mat4> GetFinalBoneMatrices() { return mFinalBoneMatrices; }

		bool mLoop;
		float mSpeed;
		float mBlendFactor;
		Ref<Animation> mNextAnimation;

	private:
		std::vector<glm::mat4> mFinalBoneMatrices;
		Ref<Animation> mCurrentAnimation;
		
		bool mPlayed;
		float mCurrentTime;
		float mDeltaTime;
	};
}

#endif