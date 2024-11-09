/******************************************************************************
/*!
\file       TimeManager.hpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 15, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef TimeManager_HPP
#define TimeManager_HPP

namespace Borealis
{
	class TimeManager
	{
	public:
		static void SetDeltaTime(float deltaTime);
		static float GetDeltaTime();
	private:
		static float m_DeltaTime;
	};
}

#endif
