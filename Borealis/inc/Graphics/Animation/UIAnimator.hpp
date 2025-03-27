#ifndef UIANIMATOR_HPP
#define UIANIMATOR_HPP

#include <Core/Core.hpp>
#include "UIAnimation.hpp"

namespace Borealis
{
	class UIAnimator
	{
	public:
		UIAnimator();
		void UpdateAnimation(float dt);
		void PlayAnimation(Ref<UIAnimation> animation);
		void StopAnimation() { mIsPlaying = false; }

		void SetLoop(bool loop) { mLoop = loop; }
		void SetSpeed(float speed) { mSpeed = speed; }
		int GetCurrentSpriteIndex() const { return mCurrentSpriteIndex; }
		Ref<UIAnimation> GetCurrentAnimation() const { return currentAnimation; }

		bool mIsPlaying = true;
		bool mLoopEnd = false;

	private:
		bool mLoop;
		bool mPlayed;
		float mSpeed;
		float mCurrentTime;
		int mCurrentSpriteIndex;
		Ref<UIAnimation> currentAnimation;
	};
}

#endif