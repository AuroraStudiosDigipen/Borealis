/******************************************************************************
/*!
\file       TagList.hpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 15, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef TagList_HPP
#define TagList_HPP
#include <string>
#include <unordered_set>
namespace Borealis
{
	class TagList
	{
	public:
		static void AddTag(std::string tag);
		static void RemoveTag(std::string tag);
		static void Clear();
		static bool HasTag(std::string tag);
	private:
		static std::unordered_set<std::string> mTags;
	};
}

#endif
