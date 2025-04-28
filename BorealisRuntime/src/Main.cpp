/******************************************************************************
/*!
\file       Main.cpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 15, 2024
\brief      Defines the entry point of the runtime

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#include <Borealis.hpp>
#include <RuntimeLayer.hpp>
#include <Core/EntryPoint.hpp>	
#include <windows.h>
#include <filesystem>
#include <cstdlib>
class Runtime : public Borealis::ApplicationManager
{
public:
	Runtime() : ApplicationManager(GetExeName())
	{
#ifndef _DIST
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
		//PushLayer(new gameLayer());
		PushLayer(new BorealisRuntime::RuntimeLayer());
	}

	~Runtime()
	{


	}

private:
	std::string GetExeName() {
		wchar_t buffer[MAX_PATH];
		DWORD result = GetModuleFileName(NULL, buffer, MAX_PATH);
		std::filesystem::path path(buffer);
		std::string filename = path.filename().string();
		if (path.has_extension())
		{
			filename = filename.substr(0, filename.find_last_of('.'));
		}
		return filename;
	}
};


Borealis::ApplicationManager* Borealis::CreateApplication()
{
	return new Runtime();
}

