/******************************************************************************
/*!
\file       Components.cpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 26, 2024
\brief      Defines the Component registrations

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>
#include <rttr/registration>
#include <Scene/ComponentRegistry.hpp>
#include <Core/LoggerSystem.hpp>

using namespace rttr;
namespace Borealis
{
    enum dataTypes
    {
        INT,
        FLOAT,
        STRING,
        BOOL,
        VEC2,
        VEC3,
        VEC4,
        MAT4
    };

    std::unordered_map<type, dataTypes> componentTypes
    {
        {type::get<int>(), dataTypes::INT},
        {type::get<float>(), dataTypes::FLOAT},
        {type::get<std::string>(), dataTypes::STRING},
        {type::get<bool>(), dataTypes::BOOL},
        {type::get<glm::vec2>(), dataTypes::VEC2},
        {type::get<glm::vec3>(), dataTypes::VEC3},
        {type::get<glm::vec4>(), dataTypes::VEC4},
        {type::get<glm::mat4>(), dataTypes::MAT4}
    };

    RTTR_REGISTRATION
    {
        registration::class_<TransformComponent>("TransformComponent")
        (metadata("Component", true))
             .constructor<>()
             .property("Translate", &TransformComponent::Translate)
             .property("Rotation", &TransformComponent::Rotation)
             .property("Scale", &TransformComponent::Scale)
             .method("GetTransform", &TransformComponent::GetTransform);

        registration::class_<SpriteRendererComponent>("SpriteRendererComponent")
            (metadata("Component", true))
            .constructor<>()
            .property("Colour", &SpriteRendererComponent::Colour)
            .property("Texture", &SpriteRendererComponent::Texture)
            .property("TilingFactor", &SpriteRendererComponent::TilingFactor);

        registration::class_<CircleRendererComponent>("CircleRendererComponent")
            (metadata("Component", true))
            .constructor<>()
            .property("Colour", &CircleRendererComponent::Colour)
            .property("Thickness", &CircleRendererComponent::thickness)
            .property("Fade", &CircleRendererComponent::fade);

        registration::class_<CameraComponent>("CameraComponent")
            (metadata("Component", true))
            .constructor<>()
            .property("Colour", &CameraComponent::Camera)
            .property("Primary", &CameraComponent::Primary)
            .property("Primary", &CameraComponent::FixedAspectRatio);
    }

    std::vector<std::string> ComponentRegistry::GetComponentNames()
    {
        std::vector<std::string> componentNames;
        for (auto& t : type::get_types())
        {
            if (t.is_class() && t.get_metadata("Component"))
            {
				componentNames.push_back(t.get_name().to_string());
			}
		}
		return componentNames;
    }

    std::vector<std::string> ComponentRegistry::GetPropertyNames(std::string componentName)
    {
        type t = type::get_by_name(componentName);
        std::vector<std::string> properties;

        for (auto prop : t.get_properties())
        {
			properties.push_back(prop.get_name().to_string());
		}
        variant r;
        return properties;
    }

    

#ifndef RegisterSetPropertyFunction
#define RegisterSetPropertyFunction(datatype) \
void Borealis::ComponentRegistry::SetPropertyInternal(const std::string& propertyName, const void*& data, const datatype& object) \
{ \
    auto t = type::get(object); \
    property prop = t.get_property(propertyName); \
    auto propType = prop.get_type(); \
    bool success = true; \
    switch (componentTypes[propType]) \
    { \
        case dataTypes::INT: \
            success = prop.set_value(object, *(int*)data); \
            break; \
        case dataTypes::FLOAT: \
            success = prop.set_value(object, *(float*)data); \
            break; \
        case dataTypes::BOOL: \
            success = prop.set_value(object, *(bool*)data); \
            break; \
        case dataTypes::STRING: \
            success = prop.set_value(object, *(std::string*)data); \
            break; \
        case dataTypes::VEC2: \
            success = prop.set_value(object, *(glm::vec2*)data); \
            break; \
        case dataTypes::VEC3: \
            success = prop.set_value(object, *(glm::vec3*)data); \
            break; \
        case dataTypes::VEC4: \
            success = prop.set_value(object, *(glm::vec4*)data); \
            break; \
        case dataTypes::MAT4: \
            success = prop.set_value(object, *(glm::mat4*)data); \
            break; \
        default: \
            BOREALIS_CORE_ASSERT(false, "Invalid data type being reflected"); \
    } \
    if (!success) \
    { \
        BOREALIS_CORE_WARN("Failed to set property value"); \
    } \
} \

#endif


#ifndef RegisterCopyPropertyFunction
#define RegisterCopyPropertyFunction(datatype) \
    void ComponentRegistry::CopyPropertyInternal(const std::string& propertyName, const datatype& src, const datatype& destination) \
    { \
        auto t = type::get(src); \
		property prop = t.get_property(propertyName); \
		auto propType = prop.get_type(); \
		bool success = true; \
        switch (componentTypes[propType]) \
        { \
			case dataTypes::INT: \
				success = prop.set_value(destination, prop.get_value(src).get_value<int>()); \
				break; \
			case dataTypes::FLOAT: \
				success = prop.set_value(destination, prop.get_value(src).get_value<float>()); \
				break; \
			case dataTypes::BOOL: \
				success = prop.set_value(destination, prop.get_value(src).get_value<bool>()); \
				break; \
			case dataTypes::STRING: \
				success = prop.set_value(destination, prop.get_value(src).get_value<std::string>()); \
				break; \
			case dataTypes::VEC2: \
				success = prop.set_value(destination, prop.get_value(src).get_value<glm::vec2>()); \
				break; \
			case dataTypes::VEC3: \
				success = prop.set_value(destination, prop.get_value(src).get_value<glm::vec3>()); \
				break; \
			case dataTypes::VEC4: \
				success = prop.set_value(destination, prop.get_value(src).get_value<glm::vec4>()); \
				break; \
			case dataTypes::MAT4: \
				success = prop.set_value(destination, prop.get_value(src).get_value<glm::mat4>()); \
				break; \
			default: \
				BOREALIS_CORE_ASSERT(false, "Invalid data type being reflected"); \
		} \
        if (!success) \
        { \
			BOREALIS_CORE_WARN("Failed to set property value"); \
		}    \
    } \

#endif


    RegisterSetPropertyFunction(TransformComponent);
    RegisterSetPropertyFunction(SpriteRendererComponent);
    RegisterSetPropertyFunction(CircleRendererComponent);
    RegisterSetPropertyFunction(CameraComponent);
    RegisterSetPropertyFunction(NativeScriptComponent);
    RegisterSetPropertyFunction(MeshFilterComponent);
    RegisterSetPropertyFunction(MeshRendererComponent);
    RegisterSetPropertyFunction(BoxColliderComponent);
    RegisterSetPropertyFunction(CapsuleColliderComponent);
    RegisterSetPropertyFunction(RigidBodyComponent);
    RegisterSetPropertyFunction(LightComponent);
    RegisterSetPropertyFunction(TextComponent);

    RegisterCopyPropertyFunction(TransformComponent);
    RegisterCopyPropertyFunction(SpriteRendererComponent);
    RegisterCopyPropertyFunction(CircleRendererComponent);
    RegisterCopyPropertyFunction(CameraComponent);
    RegisterCopyPropertyFunction(NativeScriptComponent);
    RegisterCopyPropertyFunction(MeshFilterComponent);
    RegisterCopyPropertyFunction(MeshRendererComponent);
    RegisterCopyPropertyFunction(BoxColliderComponent);
    RegisterCopyPropertyFunction(CapsuleColliderComponent);
    RegisterCopyPropertyFunction(RigidBodyComponent);
    RegisterCopyPropertyFunction(LightComponent);
    RegisterCopyPropertyFunction(TextComponent);

}