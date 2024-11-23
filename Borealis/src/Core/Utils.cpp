/***************************************************************************** /
/*!
\file		Utils.cpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 15, 2024
\brief		Defines the functions for Utilities

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *
 /******************************************************************************/
#include <BorealisPCH.hpp>
#include <commdlg.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <Core/ApplicationManager.hpp>
#include <Core/Utils.hpp>


namespace Borealis
{
	std::string FileDialogs::OpenFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[MAX_PATH] = { 0 };
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)ApplicationManager::Get().GetWindow()->GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		CHAR szInitialDir[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, szInitialDir);
		ofn.lpstrInitialDir = szInitialDir;
		if (GetOpenFileNameA(&ofn))
		{
			return ofn.lpstrFile;
		}
		return std::string();
	}
	std::string FileDialogs::SaveFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[MAX_PATH] = { 0 };
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)ApplicationManager::Get().GetWindow()->GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		CHAR szInitialDir[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, szInitialDir);
		ofn.lpstrInitialDir = szInitialDir;
		if (GetSaveFileNameA(&ofn))
		{
			return ofn.lpstrFile;
		}
		return std::string();
	}

	bool GraphicsUtils::IsDepthFormat(FramebufferTextureFormat format)
	{
		switch (format)
		{
		case FramebufferTextureFormat::Depth24Stencil8:
			return true;
		}
		return false;
	}

	unsigned GraphicsUtils::TextureTarget(bool multiSampled)
	{
		return multiSampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}

	void GraphicsUtils::CreateTextures(bool multiSampled, uint32_t* outID, size_t count)
	{
		glGenTextures((GLsizei)count, outID);
	}

	void GraphicsUtils::BindTexture(bool multiSampled, uint32_t id)
	{
		glBindTexture(TextureTarget(multiSampled), id);
	}

	void GraphicsUtils::AttachColorTexture(uint32_t id, int samples, unsigned internalformat, unsigned format, unsigned type, uint32_t width, uint32_t height, int index)
	{
		bool multisampled = samples > 1;
		if (multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalformat, width, height, GL_FALSE);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, nullptr);

			//fix ltr
			if (internalformat == GL_RED_INTEGER)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
	}

	void GraphicsUtils::AttachDepthTexture(uint32_t id, int samples, unsigned format, unsigned attachmentType, uint32_t width, uint32_t height)
	{
		bool multisampled = samples > 1;
		if (multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
	}

	glm::vec3 Math::QuatToEuler(glm::quat q, bool radians)
	{
		float yaw = atan2(2.0f * (q.y * q.z + q.w * q.x), q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);
		float pitch = asin(-2.0f * (q.x * q.z - q.w * q.y));
		float roll = atan2(2.0f * (q.x * q.y + q.w * q.z), q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z);

		if (!radians)
		{
			yaw = glm::degrees(yaw);
			pitch = glm::degrees(pitch);
			roll = glm::degrees(roll);
		}
		return glm::vec3(yaw, pitch, roll);
	}

	glm::quat Math::EulerToQuat(glm::vec3 euler, bool radians)
	{
		if (!radians)
		{
			euler.x = glm::radians(euler.x);
			euler.y = glm::radians(euler.y);
			euler.z = glm::radians(euler.z);
		}

		float pitch = euler.x;
		float yaw = euler.y;
		float roll = euler.z;

		float cy = cos(yaw * 0.5f);
		float sy = sin(yaw * 0.5f);
		float cp = cos(pitch * 0.5f);
		float sp = sin(pitch * 0.5f);
		float cr = cos(roll * 0.5f);
		float sr = sin(roll * 0.5f);

		glm::quat q;
		q.w = cr * cp * cy + sr * sp * sy;
		q.x = sr * cp * cy - cr * sp * sy;
		q.y = cr * sp * cy + sr * cp * sy;
		q.z = cr * cp * sy - sr * sp * cy;

		return glm::normalize(q);
		
	}

	void Math::MatrixDecomposition(glm::mat4* transform, glm::vec3* translate, glm::vec3* rotate, glm::vec3* scale)
	{

		*translate = glm::vec3((*transform)[3]);

		scale->x = glm::length(glm::vec3((*transform)[0]));
		scale->y = glm::length(glm::vec3((*transform)[1]));
		scale->z = glm::length(glm::vec3((*transform)[2]));


		// Normalize the basis vectors
		glm::vec3 xAxis = glm::vec3((*transform)[0]) / scale->x;
		glm::vec3 yAxis = glm::vec3((*transform)[1]) / scale->y;
		glm::vec3 zAxis = glm::vec3((*transform)[2]) / scale->z;

		glm::quat rotation = glm::quat_cast(glm::mat3(xAxis, yAxis, zAxis));
		*rotate = glm::degrees(glm::eulerAngles(rotation)); // Convert quaternion to Euler angles

	}

	std::string StringUtils::SplitAndCapitalize(const std::string& str)
	{
		std::vector<std::string> words;
		std::string currentWord;

		for (char ch : str) {
			if (std::isupper(ch)) {
				// If current word is not empty, push it to words and start a new word
				if (!currentWord.empty()) {
					words.push_back(currentWord);
				}
				currentWord = ch;  // Start new word with uppercase letter
			}
			else {
				currentWord += ch;  // Continue current word
			}
		}

		// Push the last word if it exists
		if (!currentWord.empty()) {
			words.push_back(currentWord);
		}

		// Capitalize the first letter of each word and join them with spaces
		for (auto& word : words) {
			word[0] = std::toupper(word[0]);
		}

		// Join words with spaces
		std::stringstream result;
		for (size_t i = 0; i < words.size(); ++i) {
			result << words[i];
			if (i != words.size() - 1) {
				result << " ";
			}
		}

		return result.str();
	}

}
