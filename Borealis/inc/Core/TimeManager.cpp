
/******************************************************************************
/*!
\file       TimeManager.cpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 15, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <Core/TimeManager.hpp>

namespace Borealis
{

	float TimeManager::m_DeltaTime = 0.0f;
	void TimeManager::SetDeltaTime(float deltaTime)
	{
		m_DeltaTime = deltaTime;
	}

	float TimeManager::GetDeltaTime()
	{
		return m_DeltaTime;
	}
}
