/******************************************************************************/
/*!
\file		InputSystem.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	February 19, 2024
\brief		Defines the InputSystem Class to manage input handles

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <GLFW/glfw3.h>
#include <Core/InputSystem.hpp>
#include <Core/ApplicationManager.hpp>
#include <Core/EventSystem.hpp>
#include <Core/KeyCodes.hpp>
#include <Events/EventInput.hpp>
namespace Borealis
{
	uint16_t InputSystem::KeyCurrentState[349];
	uint16_t InputSystem::KeyPrevState[349];
	double InputSystem::ScrollY = 0;
	float InputSystem::mouseDeltaY = 0;
	float InputSystem::mouseDeltaX = 0;
	std::pair<float, float> InputSystem::mouseCurrPos = { 0,0 };
	std::pair < float, float> InputSystem::mousePrevPos = { 0,0 };

	static std::vector<uint16_t> KeysToPoll
	{
			32,
			39, /* ' */
			44, /* , */
			45, /* - */
			46, /* . */
			47, /* / */

			48, /* 0 */
			49, /* 1 */
			50, /* 2 */
			51, /* 3 */
			52, /* 4 */
			53, /* 5 */
			54, /* 6 */
			55, /* 7 */
			56, /* 8 */
			57, /* 9 */

			59, /* ; */
			61, /* = */

			65,
			66,
			67,
			68,
			69,
			70,
			71,
			72,
			73,
			74,
			75,
			76,
			77,
			78,
			79,
			80,
			81,
			82,
			83,
			84,
			85,
			86,
			87,
			88,
			89,
			90,

			91,  /* [ */
			92,  /* \ */
			93,  /* ] */
			96,  /* ` */

			161, /* non-US #1 */
			162, /* non-US #2 */

			/* Function keys */
			256,
			257,
			258,
			259,
			260,
			261,
			262,
			263,
			264,
			265,
			266,
			267,
			268,
			269,
			280,
			281,
			282,
			283,
			284,
			290,
			291,
			292,
			293,
			294,
			295,
			296,
			297,
			298,
			299,
			300,
			301,
			302,
			303,
			304,
			305,
			306,
			307,
			308,
			309,
			310,
			311,
			312,
			313,
			314,

			/* Keypad */
			320,
			321,
			322,
			323,
			324,
			325,
			326,
			327,
			328,
			329,
			330,
			331,
			332,
			333,
			334,
			335,
			336,
			340,
			341,
			342,
			343,
			344,
			345,
			346,
			347,
			348
	};
	static std::vector<uint16_t> MouseToPoll
	{
			0,
			1,
			2,
			3,
			4,
			5,
			6,
			7
	};

	void InputSystem::Init()
	{
		std::memset(KeyCurrentState, 0, sizeof(KeyCurrentState));
		std::memset(KeyPrevState, 0, sizeof(KeyPrevState));
		mouseCurrPos = GetMousePos();
	}

	bool InputSystem::IsKeyPressed(int key)
	{
		return KeyCurrentState[key];
	}

	bool InputSystem::IsKeyTriggered(int key)
	{
		return !KeyPrevState[key] && KeyCurrentState[key];
	}

	bool InputSystem::IsKeyReleased(int key)
	{
		return KeyPrevState[key] && !KeyCurrentState[key];
	}

	bool InputSystem::IsMouseButtonPressed(int key)
	{
		return KeyCurrentState[key];
	}

	bool InputSystem::IsMouseButtonTriggered(int key)
	{
		return !KeyPrevState[key] && KeyCurrentState[key];
	}

	bool InputSystem::IsMouseButtonReleased(int key)
	{
		return KeyPrevState[key] && !KeyCurrentState[key];
	}

	std::pair<float, float> InputSystem::GetMousePos()
	{
		double x, y;
		glfwGetCursorPos(static_cast<GLFWwindow*>(ApplicationManager::Get().GetWindow()->GetNativeWindow()), &x, &y);
		return { static_cast<float>(x), static_cast<float>(y) };
	}

	float InputSystem::GetMouseX()
	{
		auto [x,y] = GetMousePos();
		return x;
	}
	float InputSystem::GetMouseY()
	{
		auto [x, y] = GetMousePos();
		return y;
	}
	void InputSystem::PollInput()
	{
		std::memcpy(KeyPrevState, KeyCurrentState, sizeof(KeyCurrentState));
		for (auto key : KeysToPoll)
		{
			auto state = glfwGetKey(static_cast<GLFWwindow*>(ApplicationManager::Get().GetWindow()->GetNativeWindow()), key);
			if (state == GLFW_PRESS || state == GLFW_REPEAT)
			{
				KeyCurrentState[key] = true;
			}
			else
			{
				KeyCurrentState[key] = false;
			}
		}

		for (auto mouse : MouseToPoll)
		{
			auto state = glfwGetMouseButton(static_cast<GLFWwindow*>(ApplicationManager::Get().GetWindow()->GetNativeWindow()), mouse);
			if (state == GLFW_PRESS)
			{
				KeyCurrentState[mouse] = true;
			}
			else
			{
				KeyCurrentState[mouse] = false;
			}
		}

		mousePrevPos = mouseCurrPos;
		mouseCurrPos = GetMousePos();
	}
	float InputSystem::GetMouseDeltaX()
	{
		mouseDeltaX = mouseCurrPos.first - mousePrevPos.first;
		int width = 0, height = 0;
		glfwGetWindowSize(static_cast<GLFWwindow*>(ApplicationManager::Get().GetWindow()->GetNativeWindow()), &width, &height);
		return mouseDeltaX/width;
	}
	float InputSystem::GetMouseDeltaY()
	{
		mouseDeltaY = mouseCurrPos.second - mousePrevPos.second;
		int width = 0, height = 0;
		glfwGetWindowSize(static_cast<GLFWwindow*>(ApplicationManager::Get().GetWindow()->GetNativeWindow()), &width, &height);
		return mouseDeltaY/height;
	}
	float InputSystem::GetMouseDeltaXRaw()
	{
		auto result = mouseCurrPos.first - mousePrevPos.first;
		if (result >= 5)
		{
			return 1;
		}
		else if (result <= -5)
		{
			return -1;
		}
		else
		{
			return 0.0f;
		}
	}
	float InputSystem::GetMouseDeltaYRaw()
	{
		auto result = mouseCurrPos.second - mousePrevPos.second;
		if (result >= 5)
		{
			return 1;
		}
		else if (result <= -5)
		{
			return -1;
		}
		else
		{
			return 0.0f;
		}
	}
} // End of namespace Borealis
