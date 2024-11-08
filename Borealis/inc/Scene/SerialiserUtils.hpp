/******************************************************************************
/*!
\file       SerialiserUtils.hpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 15, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef SerialiserUtils_HPP
#define SerialiserUtils_HPP
#include <Scene/Scene.hpp>
#include <Scene/Entity.hpp>
#include <yaml-cpp/yaml.h>
#include <Assets/AssetManager.hpp>
#include <Scene/ReflectionInstance.hpp>
#include <Scene/Components.hpp>
#include <Scripting/ScriptInstance.hpp>
#include <Scripting/ScriptField.hpp>
#include <Scripting/ScriptingSystem.hpp>
#include <imgui.h>

namespace YAML
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
			{
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};



	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
			{
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
			{
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<ImVec4>
	{
		static Node encode(const ImVec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, ImVec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
			{
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Borealis
{

	// Overrides:
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& vec);
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& vec);
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& vec);
	YAML::Emitter& operator<<(YAML::Emitter& out, const ImVec4& vec);

	static bool SerializeProperty(YAML::Emitter& out, rttr::property& prop, const rttr::instance& instance)
	{
		auto propType = prop.get_type();
		auto propName = prop.get_name();
		auto propValue = prop.get_value(instance);

		if (propType.is_enumeration())
		{
			out << YAML::Key << propName.to_string() << YAML::Value << propType.get_enumeration().value_to_name(propValue).to_string();
			return true;
		}

		if (propType == rttr::type::get<int>())
		{
			out << YAML::Key << propName.to_string() << YAML::Value << propValue.to_int();
			return true;
		}

		if (propType == rttr::type::get<float>())
		{
			out << YAML::Key << propName.to_string() << YAML::Value << propValue.to_float();
			return true;
		}

		if (propType == rttr::type::get<bool>())
		{
			out << YAML::Key << propName.to_string() << YAML::Value << propValue.to_bool();
			return true;
		}

		if (propType == rttr::type::get<std::string>())
		{
			out << YAML::Key << propName.to_string() << YAML::Value << propValue.to_string();
			return true;
		}

		if (propType == rttr::type::get<unsigned char>())
		{
			out << YAML::Key << propName.to_string() << YAML::Value << (unsigned)propValue.to_uint8();
			return true;
		}

		if (propType == rttr::type::get<char>())
		{
			out << YAML::Key << propName.to_string() << YAML::Value << (int)propValue.to_int8();
			return true;
		}

		if (propType == rttr::type::get<unsigned short>())
		{
			out << YAML::Key << propName.to_string() << YAML::Value << (unsigned)propValue.to_uint16();
			return true;
		}

		if (propType == rttr::type::get<short>())
		{
			out << YAML::Key << propName.to_string() << YAML::Value << (int)propValue.to_int16();
			return true;
		}

		if (propType == rttr::type::get<unsigned>())
		{
			out << YAML::Key << propName.to_string() << YAML::Value << propValue.to_uint32();
			return true;
		}

		if (propType == rttr::type::get<long long>())
		{
			out << YAML::Key << propName.to_string() << YAML::Value << propValue.to_int64();
			return true;
		}

		if (propType == rttr::type::get<unsigned long long>())
		{
			out << YAML::Key << propName.to_string() << YAML::Value << propValue.to_uint64();
			return true;
		}

		if (propType == rttr::type::get<UUID>())
		{
			UUID data = propValue.get_value<UUID>();
			out << YAML::Key << propName.to_string() << YAML::Value << (uint64_t)data;
			return true;
		}

		if (propType == rttr::type::get<double>())
		{
			out << YAML::Key << propName.to_string() << YAML::Value << propValue.to_double();
			return true;
		}

		if (propType == rttr::type::get<glm::vec2>())
		{
			out << YAML::Key << propName.to_string() << YAML::Value << propValue.get_value<glm::vec2>();
			return true;
		}

		if (propType == rttr::type::get<glm::vec3>())
		{
			out << YAML::Key << propName.to_string() << YAML::Value << propValue.get_value<glm::vec3>();
			return true;
		}

		if (propType == rttr::type::get<glm::vec4>())
		{
			out << YAML::Key << propName.to_string() << YAML::Value << propValue.get_value<glm::vec4>();
			return true;
		}

		if (propType == rttr::type::get<std::unordered_set<std::string>>())
		{
			std::unordered_set<std::string> strings = propValue.get_value<std::unordered_set<std::string>>();
			out << YAML::Key << propName.to_string() << YAML::Value << YAML::BeginSeq;
			for (auto& string : strings)
			{
				out << string;
			}
			out << YAML::EndSeq;
			return true;
		}

		if (propType.get_wrapped_type().is_valid())
		{
			auto wrappedType = propType.get_wrapped_type();
			if (wrappedType.is_derived_from<Asset>())
			{
				out << YAML::Key << propName.to_string() << YAML::Value << propValue.get_value<Ref<Asset>>()->mAssetHandle;
				return true;
			}
		}

		if (propType.is_class() && propType.is_valid()) // all custom classes
		{
			out << YAML::Key << propName.to_string() << YAML::BeginMap;
			auto properties = propType.get_properties();
			for (auto nestedProperty : properties)
			{
				SerializeProperty(out, nestedProperty, prop.get_value(instance));
			}
			out << YAML::EndMap;
			return true;
		}

		return false;
	}



	template <typename Component>
	static void SerializeComponent(YAML::Emitter& out, Component& component)
	{
		ReflectionInstance rInstance(component);
		out << YAML::Key << rInstance.get_type().get_name().to_string();
		out << YAML::BeginMap;
		auto properties = rInstance.get_type().get_properties();
		for (auto prop : properties)
		{
			SerializeProperty(out, prop, rInstance);
		}
		out << YAML::EndMap;
	}


	static bool DeserialiseProperty(YAML::Node data, rttr::property& prop, rttr::instance& instance)
	{
		auto propName = prop.get_name();
		auto propData = data[propName.to_string().c_str()]; // entity["TransformComponent"]["Translate"]
		auto propType = prop.get_type();
		if (propData)
		{
			if (propType.is_enumeration())
			{
				auto datastr = propData.as<std::string>();
				prop.set_value(instance, propType.get_enumeration().name_to_value(propData.as<std::string>()));
				return true;
			}
			if (propType == rttr::type::get<int>())
			{
				prop.set_value(instance, propData.as<int>());
				return true;
			}

			if (propType == rttr::type::get<float>())
			{
				prop.set_value(instance, propData.as<float>());
				return true;
			}

			if (propType == rttr::type::get<bool>())
			{
				prop.set_value(instance, propData.as<bool>());
				return true;
			}

			if (propType == rttr::type::get<std::string>())
			{
				prop.set_value(instance, propData.as<std::string>());
				return true;
			}

			if (propType == rttr::type::get<unsigned char>())
			{
				prop.set_value(instance, propData.as<unsigned char>());
				return true;
			}

			if (propType == rttr::type::get<char>())
			{
				prop.set_value(instance, propData.as<char>());
				return true;
			}

			if (propType == rttr::type::get<unsigned short>())
			{
				prop.set_value(instance, propData.as<unsigned short>());
				return true;
			}

			if (propType == rttr::type::get<short>())
			{
				prop.set_value(instance, propData.as<short>());
				return true;
			}

			if (propType == rttr::type::get<unsigned>())
			{
				prop.set_value(instance, propData.as<unsigned>());
				return true;
			}

			if (propType == rttr::type::get<long long>())
			{
				prop.set_value(instance, propData.as<long long>());
				return true;
			}

			if (propType == rttr::type::get<unsigned long long>())
			{
				prop.set_value(instance, propData.as<unsigned long long>());
				return true;
			}

			if (propType == rttr::type::get<double>())
			{
				prop.set_value(instance, propData.as<double>());
				return true;
			}

			if (propType == rttr::type::get<glm::vec2>())
			{
				prop.set_value(instance, propData.as<glm::vec2>());
				return true;
			}

			if (propType == rttr::type::get<glm::vec3>())
			{
				prop.set_value(instance, propData.as<glm::vec3>());
				return true;
			}

			if (propType == rttr::type::get<glm::vec4>())
			{
				prop.set_value(instance, propData.as<glm::vec4>());
				return true;
			}

			if (propType == rttr::type::get<UUID>())
			{
				prop.set_value(instance, UUID(propData.as<uint64_t>()));
				return true;
			}

			if (propType == rttr::type::get<std::unordered_set<std::string>>())
			{
				std::unordered_set<std::string> strings;
				for (auto string : propData)
				{
					strings.insert(string.as<std::string>());
				}
				prop.set_value(instance, strings);
				return true;
			}


			if (propType == rttr::type::get<Ref<Model>>())
			{
				prop.set_value(instance, rttr::variant(AssetManager::GetAsset<Model>(propData.as<uint64_t>())));
				return true;
			}

			if (propType == rttr::type::get<Ref<Material>>())
			{
				prop.set_value(instance, rttr::variant(AssetManager::GetAsset<Material>(propData.as<uint64_t>())));
				return true;
			}

			if (propType.is_class() && propType.is_valid()) // all custom classes
			{
				auto properties = propType.get_properties();
				rttr::variant oldVariant = prop.get_value(instance);
				rttr::instance oldInstance(oldVariant);
				if (oldInstance.is_valid())
				{
					for (auto nestedProperty : properties)
					{
						DeserialiseProperty(data[propName.to_string()], nestedProperty, oldInstance);
					}

					prop.set_value(instance, oldVariant);
				}
				return true;
			}

			// other types
		}

		return false;
	}

	template <typename Component>
	static bool DeserialiseComponent(YAML::detail::iterator_value& data, Entity& entity)
	{
		auto type = rttr::type::get<Component>();
		auto componentName = type.get_name().to_string();
		auto componentData = data[componentName];  //entity["TransformComponent"]

		if (componentData)
		{
			if (!entity.HasComponent<Component>())
			{
				entity.AddComponent<Component>();
			}
			auto& component = entity.GetComponent<Component>();
			ReflectionInstance rInstance(component);

			auto properties = rInstance.get_type().get_properties();
			for (auto prop : properties)
			{
				DeserialiseProperty(componentData, prop, rInstance);
			}
		}

		return true;
	}
}

#endif
