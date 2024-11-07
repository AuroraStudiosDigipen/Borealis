/******************************************************************************
/*!
\file       TagList.cpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 07, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include "Core/TagList.hpp"

namespace Borealis
{
	std::unordered_set<std::string> TagList::mTags;
	void TagList::AddTag(std::string tag)
	{
		mTags.insert(tag);
	}
	void TagList::RemoveTag(std::string tag)
	{
		mTags.erase(tag);
	}
	void TagList::Clear()
	{
		mTags.clear();
	}
	bool TagList::HasTag(std::string tag)
	{
		return mTags.find(tag) != mTags.end();
	}
}

