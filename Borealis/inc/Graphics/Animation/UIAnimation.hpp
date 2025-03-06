#ifndef UIANIMATION_HPP
#define UIANIMATION_HPP

#include <Core/Core.hpp>

#include "Graphics/Texture.hpp"

namespace Borealis
{
	struct Sprite
	{
		glm::vec2 offset;
	};

	class UIAnimation
	{
	public:
		UIAnimation(Ref<Texture2D> spritesheet, float duration, int numRow, int numCol, int numSprites);

		void LoadAnimation();

		glm::vec2 GetSpriteSize() { return spriteSize; }
		std::vector<Sprite>& GetSprites() { return sprites; }
		float GetDuration() { return mDuration; }

	private:
		Ref<Texture2D> texture;
		std::vector<Sprite> sprites;
		float mDuration;
		glm::vec2 spriteSize;
		int mNumRow;
		int mNumCol;
		int mNumSprites;
	};
}

#endif