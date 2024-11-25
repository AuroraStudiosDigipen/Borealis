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
            .property("Center", &BoxColliderComponent::center)
            .property("Size", &BoxColliderComponent::size);

        registration::class_<CapsuleColliderComponent>("Capsule Collider Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Is Trigger", &CapsuleColliderComponent::isTrigger)
            .property("Provides Contact", &CapsuleColliderComponent::providesContact)
            .property("Radius", &CapsuleColliderComponent::radius)
            .property("Height", &CapsuleColliderComponent::height)
            .property("Direction", &CapsuleColliderComponent::direction);


        registration::class_<SphereColliderComponent>("Sphere Collider Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Is Trigger", &SphereColliderComponent::isTrigger)
            .property("Provides Contact", &CapsuleColliderComponent::providesContact)
            .property("Center", &SphereColliderComponent::center)
            .property("Radius", &SphereColliderComponent::radius);


        registration::enumeration<SceneCamera::CameraType>("Camera Type")
        (
            value("Orthographic", SceneCamera::CameraType::Orthographic),
            value("Perspective", SceneCamera::CameraType::Perspective)
        );

        registration::class_<SceneCamera>("Scene Camera")
            (metadata("SubComponent", true))
            .constructor<>()
            .property("Camera Type", &SceneCamera::GetCameraType, &SceneCamera::SetCameraType)
            .property("Perspective Near Clip", &SceneCamera::GetPerspNear, &SceneCamera::SetPerspNear)
            (metadata("Dependency", "Camera Type"), metadata("Visible for", "Perspective"))
            .property("Perspective Far Clip", &SceneCamera::GetPerspFar, &SceneCamera::SetPerspFar)
            (metadata("Dependency", "Camera Type"), metadata("Visible for", "Perspective"))
            .property("Perspective FOV", &SceneCamera::GetPerspFOV, &SceneCamera::SetPerspFOV)
            (metadata("Dependency", "Camera Type"), metadata("Visible for", "Perspective"))
            .property("Orthographic Near Clip", &SceneCamera::GetOrthoNear, &SceneCamera::SetOrthoNear)
            (metadata("Dependency", "Camera Type"), metadata("Visible for", "Orthographic"))
            .property("Orthographic Far Clip", &SceneCamera::GetOrthoFar, &SceneCamera::SetOrthoFar)
            (metadata("Dependency", "Camera Type"), metadata("Visible for", "Orthographic"))
            .property("Orthographic Size", &SceneCamera::GetOrthoSize, &SceneCamera::SetOrthoSize)
            (metadata("Dependency", "Camera Type"), metadata("Visible for", "Orthographic"));
            
        registration::class_<CameraComponent>("Camera Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Primary Camera", &CameraComponent::Primary)
            .property("Fixed Aspect Ratio", &CameraComponent::FixedAspectRatio)
            .property("Camera", &CameraComponent::Camera);

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
            .property("Type", &LightComponent::type)
            .property("Range", &LightComponent::range)
            (metadata("Dependency", "Type"), metadata("Visible for", "Point,Spot"))
            .property("Spot Angle", &LightComponent::spotAngle)
            (metadata("Dependency", "Type"), metadata("Visible for", "Spot"))
            .property("Color", &LightComponent::color)
            (metadata("Colour", true))
            .property("Intensity", &LightComponent::Intensity)
            .property("CastShadow", &LightComponent::castShadow);


        registration::class_<MeshFilterComponent>("Mesh Filter Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Model", &MeshFilterComponent::Model);

        registration::class_<MeshRendererComponent>("Mesh Renderer Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Material", &MeshRendererComponent::Material)
            .property("Cast Shadow", &MeshRendererComponent::castShadow);

        registration::class_<SkinnedMeshRendererComponent>("Skinned Mesh Renderer Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Skinnned Model", &SkinnedMeshRendererComponent::SkinnnedModel)
            .property("Material", &SkinnedMeshRendererComponent::Material);

        registration::class_<AnimatorComponent>("Animator Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Animation", &AnimatorComponent::animation)
            .property("Animator", &AnimatorComponent::animator);

        registration::enumeration<RigidBodyType>("Rigidbody Collider Type")
            (
                value("Box", RigidBodyType::Box),
                value("Sphere", RigidBodyType::Sphere),
				value("Capsule", RigidBodyType::Capsule)
                );

		registration::enumeration<MovementType>("Rigidbody Movement Type")
			(
				value("Static", MovementType::Static),
				value("Dynamic", MovementType::Dynamic),
				value("Kinematic", MovementType::Kinematic)
				);

		registration::enumeration<CapsuleColliderComponent::Direction>("Capsule Direction")
			(
				value("X", CapsuleColliderComponent::Direction::X),
				value("Y", CapsuleColliderComponent::Direction::Y),
				value("Z", CapsuleColliderComponent::Direction::Z)
				);

        registration::class_<RigidBodyComponent>("Rigid Body Component")
            (metadata("Component", true))
            .constructor<>()
			.property("Movement", &RigidBodyComponent::movement)

			//.property("Dynamic", &RigidBodyComponent::dynamicBody)

            //.property("Radius", &RigidBodyComponent::radius)
            //(metadata("Dependency", "Shape"), metadata("Visible for", "Sphere"))

            //.property("Size", &RigidBodyComponent::size)
            //(metadata("Dependency", "Shape"), metadata("Visible for", "Box"))
            //
            //.property("Radius ", &RigidBodyComponent::radius)
            //(metadata("Dependency", "Shape"), metadata("Visible for", "Capsule"))
            //
            //.property("Half Height", &RigidBodyComponent::halfHeight)
            //(metadata("Dependency", "Shape"), metadata("Visible for", "Capsule"))
            
            .property("Friction", &RigidBodyComponent::friction)
            
            .property("Bounciness", &RigidBodyComponent::bounciness);

        registration::class_<CharacterControlComponent>("Character Controller Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Mass", &CharacterControlComponent::mass)
            .property("Max Slope Angle", &CharacterControlComponent::slopeAngle)
            .property("Max Strength", &CharacterControlComponent::strength)
            .property("Inertia", &CharacterControlComponent::enableInertia)
            .property("Sliding", &CharacterControlComponent::sliding)
            .property("Move In Air", &CharacterControlComponent::moveInAir);

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
            .property("Font Size", &TextComponent::fontSize)
            .property("Colour", &TextComponent::colour)
            (metadata("Colour", true));

        registration::class_<TagComponent>("Tag Component")
            (metadata("Component", true))
            .constructor<>()
            .property("IsActive", &TagComponent::active)
            .property("Tag", &TagComponent::Tag)
            .property("Layer", &TagComponent::mLayer);

        registration::class_<TransformComponent>("Transform Component")
            (metadata("Component", true))
            .constructor<>()
            .property("Translate", &TransformComponent::Translate)
            .property("Rotation", &TransformComponent::Rotation)
            .property("Scale", &TransformComponent::Scale)
            (metadata("Min", 1.f))
            .property("ParentID", &TransformComponent::ParentID)
            (metadata("Hide", true))
            .property("ChildrenID", &TransformComponent::ChildrenID)
            (metadata("Hide", true))
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
    RegisterSetPropertyFunction(SkinnedMeshRendererComponent);
    RegisterSetPropertyFunction(AnimatorComponent);
    RegisterSetPropertyFunction(BoxColliderComponent);
    RegisterSetPropertyFunction(CapsuleColliderComponent);
    RegisterSetPropertyFunction(SphereColliderComponent);
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
    RegisterCopyPropertyFunction(SkinnedMeshRendererComponent);
    RegisterCopyPropertyFunction(AnimatorComponent);
    RegisterCopyPropertyFunction(BoxColliderComponent);
    RegisterCopyPropertyFunction(CapsuleColliderComponent);
    RegisterCopyPropertyFunction(SphereColliderComponent);
    RegisterCopyPropertyFunction(RigidBodyComponent);
    RegisterCopyPropertyFunction(LightComponent);
    RegisterCopyPropertyFunction(TextComponent);
}