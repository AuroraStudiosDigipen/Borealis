#pragma once

#ifndef BUTTON_HPP
#define BUTTON_HPP
#include <Core/Core.hpp>

namespace Borealis
{
	class ButtonSystem
	{
	public:
		static void Update();
		static void SetMousePos(glm::vec2 pos) { mMousePos = pos; }

	private:
		static bool mIsActive;
		static glm::vec2 mMousePos;
	};
}

#endif
