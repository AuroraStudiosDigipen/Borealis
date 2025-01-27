#include "BorealisPCH.hpp"
#include "Graphics/UI/Button.hpp"

#include "Core/ApplicationManager.hpp"
#include "Core/InputSystem.hpp"
#include "Scene/SceneManager.hpp"

namespace Borealis
{
	glm::vec2 ButtonSystem::mMousePos;
	bool ButtonSystem::mIsActive;

	bool IsMouseInButton(glm::vec2 mousePos, glm::vec2 buttonPos, glm::vec2 buttonSize)
	{
		glm::vec2 topLeft = buttonPos - (buttonSize * 0.5f);

		return (mousePos.x >= topLeft.x && mousePos.x <= topLeft.x + buttonSize.x &&
			mousePos.y >= topLeft.y && mousePos.y <= topLeft.y + buttonSize.y);
	}

	void ButtonSystem::Update()
	{
		if (!ApplicationManager::Get().GetWindow()->GetCursorVisibility()) return;

		auto group = SceneManager::GetActiveScene()->GetRegistry().group<ButtonComponent, TransformComponent>();

		auto mousePos = mMousePos;
		bool mouseDown = InputSystem::IsMouseButtonPressed(0);

        for (auto entity : group)
        {
            auto [button, transform] = group.get<ButtonComponent, TransformComponent>(entity);

            if (!button.isActive) continue;

            glm::vec2 translate = { transform.Translate.x, transform.Translate.y };
            glm::vec2 scale = { transform.Scale.x, transform.Scale.y };

            bool mouseInButton = IsMouseInButton(mousePos, translate, scale);

            bool wasHovered = button.hovered;
            button.hovered = mouseInButton;

            if (!wasHovered && button.hovered)
            {
                button.onHover();
            }

            // Click Detection
            if (mouseDown && mouseInButton)
            {
                if (!button.clicked)
                {
                    button.clicked = true;
                    button.onClick();
                }
            }
            else
            {
                if (button.clicked)
                {
                    button.clicked = false;
                    if (mouseInButton)
                    {
                        button.released = true;
                        button.onRelease();
                    }
                }
            }

            button.released = false;
        }
	}
}
