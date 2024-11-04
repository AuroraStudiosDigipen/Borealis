/******************************************************************************
/*!
\file       NodeHeaderCodeFormat.hpp
\author     Joey Chua
\par        email: joeyjunyu.c@digipen.edu
\date       September 15, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef NodeHeaderCodeFormat_HPP
#define NodeHeaderCodeFormat_HPP
#include <string>
namespace Borealis
{
	class NodeHeaderCodeFormat
	{
	public:
		static std::string GenerateHeaderCode(const std::string& className, const std::string& baseClassName, int nodeTypeIndex);
		static std::string GenerateSourceCode(const std::string& className, int nodeTypeIndex);
		static std::string GetCurrentDate();
		static void WriteToFile(const std::string& filePath, const std::string& content);
		static void ReplaceAll(std::string& str, const std::string& from, const std::string& to);
	};
}

#endif
