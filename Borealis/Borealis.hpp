/******************************************************************************/
/*!
\file		Borealis.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	February 15, 2024
\brief		Declares the includes for the engine. For use only by aplpication.

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef BOREALIS_HPP
#define BOREALIS_HPP

#include <Core/ApplicationManager.hpp>
#include <Core/LoggerSystem.hpp>
#include <Core/Layer.hpp>
#include <Core/InputSystem.hpp>
#include <Core/KeyCodes.hpp>
#include <Core/Utils.hpp>
#include <Core/Project.hpp>
#include <Core/Bitset32.hpp>

#include <Scene/OrthographicCameraController.hpp>
#include <Scene/Scene.hpp>
#include <Scene/ScriptEntity.hpp>
#include <Scene/Components.hpp>
#include <Scene/Entity.hpp>
#include <Scene/SceneManager.hpp>

#include <Scripting/ScriptingSystem.hpp>
#include <Scripting/ScriptInstance.hpp>


#include <ImGui/ImGuiLayer.hpp>
#include <ImGui/ImGuiFontLib.hpp>

#include <Graphics/Renderer.hpp>
#include <Graphics/Renderer2D.hpp>
#include <Graphics/Framebuffer.hpp>
#include <Graphics/RenderCommand.hpp>
#include <Graphics/Buffer.hpp>
#include <Graphics/VertexArray.hpp>
#include <Graphics/Shader.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/SubTexture2D.hpp>
#include <Graphics/Mesh.hpp>
#include <Graphics/OrthographicCamera.hpp>	

#endif