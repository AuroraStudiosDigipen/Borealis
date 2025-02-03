/******************************************************************************
/*!
\file       ComponentRegistry.hpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 27, 2024
\brief      Declares the Component Registry

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef ComponentRegistry_HPP
#define ComponentRegistry_HPP
#include <vector>
#include <string>
#include <Scene/Components.hpp>
namespace Borealis
{
	class ComponentRegistry
	{
	public:
		static std::vector<std::string> GetPropertyNames(std::string componentName);
		static std::vector<std::string> GetComponentNames();
		template <typename Component>
		static void SetPropertyValue(const std::string& propertyName, const void*& data, const Component& object)
		{
			SetPropertyInternal(propertyName, data, object);
		}

		template <typename Component>
		static void CopyPropertyValue(const std::string& propertyName, const Component& src, const Component& dest)
		{
			CopyPropertyInternal(propertyName, src, dest);
		}
		
	private:
		static void SetPropertyInternal(const std::string& propertyName, const void*& data, const TransformComponent& object);
		static void SetPropertyInternal(const std::string& propertyName, const void*& data, const SpriteRendererComponent& object);
		static void SetPropertyInternal(const std::string& propertyName, const void*& data, const CircleRendererComponent& object);
		static void SetPropertyInternal(const std::string& propertyName, const void*& data, const CameraComponent& object);
		static void SetPropertyInternal(const std::string& propertyName, const void*& data, const NativeScriptComponent& object);
		static void SetPropertyInternal(const std::string& propertyName, const void*& data, const MeshFilterComponent& object);
		static void SetPropertyInternal(const std::string& propertyName, const void*& data, const MeshRendererComponent& object);
		static void SetPropertyInternal(const std::string& propertyName, const void*& data, const SkinnedMeshRendererComponent& object);
		static void SetPropertyInternal(const std::string& propertyName, const void*& data, const AnimatorComponent& object);
		static void SetPropertyInternal(const std::string& propertyName, const void*& data, const BoxColliderComponent& object);
		static void SetPropertyInternal(const std::string& propertyName, const void*& data, const CapsuleColliderComponent& object);
		static void SetPropertyInternal(const std::string& propertyName, const void*& data, const TaperedCapsuleColliderComponent& object);
		static void SetPropertyInternal(const std::string& propertyName, const void*& data, const SphereColliderComponent& object);
		static void SetPropertyInternal(const std::string& propertyName, const void*& data, const RigidBodyComponent& object);
		static void SetPropertyInternal(const std::string& propertyName, const void*& data, const LightComponent& object);
		static void SetPropertyInternal(const std::string& propertyName, const void*& data, const TextComponent& object);
		static void SetPropertyInternal(const std::string& propertyName, const void*& data, const OutLineComponent& object);

		static void CopyPropertyInternal(const std::string& propertyName, const TransformComponent& src, const TransformComponent& dest);
		static void CopyPropertyInternal(const std::string& propertyName, const SpriteRendererComponent& src, const SpriteRendererComponent& dest);
		static void CopyPropertyInternal(const std::string& propertyName, const CircleRendererComponent& src, const CircleRendererComponent& dest);
		static void CopyPropertyInternal(const std::string& propertyName, const CameraComponent& src, const CameraComponent& dest);
		static void CopyPropertyInternal(const std::string& propertyName, const NativeScriptComponent& src, const NativeScriptComponent& dest);
		static void CopyPropertyInternal(const std::string& propertyName, const MeshFilterComponent& src, const MeshFilterComponent& dest);
		static void CopyPropertyInternal(const std::string& propertyName, const MeshRendererComponent& src, const MeshRendererComponent& dest);
		static void CopyPropertyInternal(const std::string& propertyName, const SkinnedMeshRendererComponent& src, const SkinnedMeshRendererComponent& dest);
		static void CopyPropertyInternal(const std::string& propertyName, const AnimatorComponent& src, const AnimatorComponent& dest);
		static void CopyPropertyInternal(const std::string& propertyName, const BoxColliderComponent& src, const BoxColliderComponent& dest);
		static void CopyPropertyInternal(const std::string& propertyName, const CapsuleColliderComponent& src, const CapsuleColliderComponent& dest);
		static void CopyPropertyInternal(const std::string& propertyName, const TaperedCapsuleColliderComponent& src, const TaperedCapsuleColliderComponent& dest);
		static void CopyPropertyInternal(const std::string& propertyName, const SphereColliderComponent& src, const SphereColliderComponent& dest);
		static void CopyPropertyInternal(const std::string& propertyName, const RigidBodyComponent& src, const RigidBodyComponent& dest);
		static void CopyPropertyInternal(const std::string& propertyName, const LightComponent& src, const LightComponent& dest);
		static void CopyPropertyInternal(const std::string& propertyName, const TextComponent& src, const TextComponent& dest);
		static void CopyPropertyInternal(const std::string& propertyName, const OutLineComponent& src, const OutLineComponent& dest);
	};
}

#endif
