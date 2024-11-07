/******************************************************************************
/*!
\file       EditerComponentRegistry.hpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 15, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef EditerComponentRegistry_HPP
#define EditerComponentRegistry_HPP
#include <string>
#include <vector>
namespace Borealis
{
	// DO NOT USE THIS CLASS -> JUST TO ENSURE REGISTRATION COMPILES
	class EditorComponentRegistry
	{
	public:
		static std::vector<std::string> GetPropertyNames(std::string componentName);
		static std::vector<std::string> GetComponentNames();

	private:
		static std::vector<std::string> mComponentNames;
		static std::vector<std::vector<std::string>> mPropertyNames;
	};
}

#endif
