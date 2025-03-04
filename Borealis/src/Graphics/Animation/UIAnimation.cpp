#include "BorealisPCH.hpp"
#include "Graphics/Animation/UIAnimation.hpp"

namespace Borealis
{
	UIAnimation::UIAnimation(Ref<Texture2D> spritesheet, float duration, int numRow, int numCol)
	: texture(spritesheet), mDuration(duration)
	{
        spriteSize = { spritesheet->GetWidth(), spritesheet->GetHeight() };

        for (int row = 0; row < numRow; row++)
        {
            for (int col = 0; col < numCol; col++)
            {
                glm::vec2 offset = { col * spriteSize.x, row * spriteSize.y };
                sprites.push_back({ offset });
            }
        }
	}
}
