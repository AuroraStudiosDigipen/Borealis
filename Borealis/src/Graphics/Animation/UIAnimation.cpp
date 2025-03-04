#include "BorealisPCH.hpp"
#include "Graphics/Animation/UIAnimation.hpp"

namespace Borealis
{
	UIAnimation::UIAnimation(Ref<Texture2D> spritesheet, float duration, int numRow, int numCol, int numSprites)
	: texture(spritesheet), mDuration(duration), mNumRow(numRow), mNumCol(numCol), mNumSprites(numSprites) {}

    void UIAnimation::LoadAnimation()
    {
        if (!texture) { return; }
        spriteSize = { 1.f / static_cast<float>(mNumCol), 1.f / static_cast<float>(mNumRow) };

        int spriteCount = 0;
        for (int row = 0; row < mNumRow; row++)
        {
            for (int col = 0; col < mNumCol; col++)
            {
                if (spriteCount >= mNumSprites)
                    return;

                glm::vec2 offset = { col * spriteSize.x, row * spriteSize.y };
                sprites.push_back({ offset });
                spriteCount++;
            }
        }
    }
}
