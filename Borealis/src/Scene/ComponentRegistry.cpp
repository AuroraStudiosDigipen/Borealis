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
#include <entt.hpp>
#include <rttr/registration>
#include <Scene/ComponentRegistry.hpp>
#include <Core/LoggerSystem.hpp>

using namespace rttr;
namespace Borealis
{
    RTTR_REGISTRATION
    {
         registration::class_<AudioListenerComponent>("Audio Listener Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Is Audio Listener", &AudioListenerComponent::isAudioListener);

        registration::class_<AudioSourceComponent>("Audio Source Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Is Looping", &AudioSourceComponent::isLoop)
            .property("Is Mute", &AudioSourceComponent::isMute)
            .property("Is Playing", &AudioSourceComponent::isPlaying)
            .property("Volume", &AudioSourceComponent::Volume)
            .property("Audio", &AudioSourceComponent::audio);

        registration::class_<BehaviourTreeComponent>("Behaviour Tree Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Behaviour Tree", &BehaviourTreeComponent::mBehaviourTrees);

        registration::class_<BoxColliderComponent>("Box Collider Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Is Trigger", &BoxColliderComponent::isTrigger)
            .property("Provides Contact", &BoxColliderComponent::providesContact)
            .property("Center", &BoxColliderComponent::Center)
            .property("Size", &BoxColliderComponent::Size);

        registration::class_<CameraComponent>("Camera Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Primary Camera", &CameraComponent::Primary)
            .property("Fixed Aspect Ratio", &CameraComponent::FixedAspectRatio);

        registration::class_<CapsuleColliderComponent>("Capsule Collider Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Is Trigger", &CapsuleColliderComponent::isTrigger)
            .property("Provides Contact", &CapsuleColliderComponent::providesContact)
            .property("Radius", &CapsuleColliderComponent::radius)
            .property("Height", &CapsuleColliderComponent::height)
            .property("Direction", &CapsuleColliderComponent::direction);

        registration::class_<CircleRendererComponent>("Circle Renderer Component")
            .constructor<>()
            .property("Colour", &CircleRendererComponent::Colour)
                (metadata("Colour", true))
            .property("Thickness", &CircleRendererComponent::thickness)
            .property("Fade", &CircleRendererComponent::fade);

        registration::class_<IDComponent>("ID Component")
            (metadata("Component", true))
            .constructor<>()
            .property("ID", &IDComponent::ID);

        registration::enumeration<LightComponent::Type>("Light Type")
            (
                value("Directional", LightComponent::Type::Directional),
                value("Point", LightComponent::Type::Point),
                value("Spot", LightComponent::Type::Spot)
                );

        registration::class_<LightComponent>("Light Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Offset", &LightComponent::offset)
            .property("Inner Outer Spot", &LightComponent::InnerOuterSpot)
            (metadata("Dependency", "Type"), metadata("Visible for", "Spot"))
            .property("Type", &LightComponent::type)
            .property("Direction", &LightComponent::direction)
            (metadata("Dependency", "Type"), metadata("Visible for", "Directional"))
            .property("Ambient", &LightComponent::ambient)
            (metadata("Colour", true))
            .property("Diffuse", &LightComponent::diffuse)
            (metadata("Colour", true))
            .property("Specular", &LightComponent::specular)
            (metadata("Colour", true))
            .property("Quadratic", &LightComponent::quadratic)
            .property("Linear", &LightComponent::linear);

        registration::class_<MeshFilterComponent>("Mesh Filter Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Model", &MeshFilterComponent::Model);

        registration::class_<MeshRendererComponent>("Mesh Renderer Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Material", &MeshRendererComponent::Material)
            .property("Cast Shadow", &MeshRendererComponent::castShadow);

        registration::enumeration<RigidBodyType>("Rigidbody Collider Type")
            (
                value("Box", RigidBodyType::Box),
                value("Sphere", RigidBodyType::Circle)
                );

        registration::class_<RigidBodyComponent>("Rigid Body Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Is Box", &RigidBodyComponent::isBox)
            .property("Radius ", &RigidBodyComponent::radius)
            (metadata("Dependency", "Is Box"), metadata("Visible for", "Sphere"))
            .property("HalfExtent ", &RigidBodyComponent::radius)
            (metadata("Dependency", "Is Box"), metadata("Visible for", "Box"));

        registration::class_<SpriteRendererComponent>("Sprite Renderer Component")
            .constructor<>()
            .property("Colour", &SpriteRendererComponent::Colour)
            (metadata("Colour", true))
            .property("Texture", &SpriteRendererComponent::Texture)
            .property("Tiling Factor", &SpriteRendererComponent::TilingFactor);

        registration::class_<TextComponent>("Text Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Text", &TextComponent::text)
            .property("Font", &TextComponent::font)
            .property("Font Size", &TextComponent::fontSize);

        registration::class_<TagComponent>("Tag Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Tag", &TagComponent::Tag);

        registration::class_<TransformComponent>("Transform Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Translate", &TransformComponent::Translate)
            .property("Rotation", &TransformComponent::Rotation)
            .property("Scale", &TransformComponent::Scale)
            (metadata("Min", 1.f))
            .method("GetTransform", &TransformComponent::GetTransform);
    }

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