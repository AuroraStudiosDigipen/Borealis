#include "BorealisPCH.hpp"
#include "Graphics/Animation/UIAnimator.hpp"

namespace Borealis
{
	UIAnimator::UIAnimator() : mCurrentTime(0.f), mCurrentSpriteIndex(0), currentAnimation(nullptr) {}

	void UIAnimator::UpdateAnimation(float dt)
	{
        if (!currentAnimation || !mIsPlaying) return;

        int numSprites = currentAnimation->GetSprites().size();
        if (!numSprites) return;

        float animDuration = currentAnimation->GetDuration();
        mCurrentTime += dt * mSpeed;

        if (mLoop)
        {
            if (mCurrentTime >= animDuration)
            {
                mCurrentTime = fmod(mCurrentTime, animDuration);
                mLoopEnd = true; // Loop just completed
            }
            else
            {
                mLoopEnd = false;
            }
        }
        else
        {
            if (mCurrentTime >= animDuration)
            {
                mCurrentTime = animDuration;
                mLoopEnd = true;
                mIsPlaying = false;
            }
            else
            {
                mLoopEnd = false;
            }
        }

        float frameDuration = animDuration / numSprites;
        mCurrentSpriteIndex = static_cast<int>(mCurrentTime / frameDuration);
        if (mCurrentSpriteIndex >= numSprites)
            mCurrentSpriteIndex = numSprites - 1;
	}

    void UIAnimator::PlayAnimation(Ref<UIAnimation> animation)
    {
		currentAnimation = animation;
        mCurrentTime = 0.f;
        mCurrentSpriteIndex = 0;
    }

}
