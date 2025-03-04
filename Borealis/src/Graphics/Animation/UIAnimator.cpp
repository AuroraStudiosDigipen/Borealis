#include "BorealisPCH.hpp"
#include "Graphics/Animation/UIAnimator.hpp"

namespace Borealis
{
	UIAnimator::UIAnimator() : mCurrentTime(0.0f), mCurrentSpriteIndex(0) {}

	void UIAnimator::UpdateAnimation(float dt)
	{
        int numSprites = currentAnimation->GetSprites().size();
        if (numSprites == 0)
            return;

        float animDuration = currentAnimation->GetDuration();
        mCurrentTime += dt * mSpeed;

        if (mLoop)
            mCurrentTime = fmod(mCurrentTime, animDuration);
        else if (mCurrentTime > animDuration)
            mCurrentTime = animDuration;

        float frameDuration = animDuration / numSprites;
        mCurrentSpriteIndex = static_cast<int>(mCurrentTime / frameDuration);
        if (mCurrentSpriteIndex >= numSprites)
            mCurrentSpriteIndex = numSprites - 1;
	}
}
