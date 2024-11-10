/******************************************************************************
/*!
\file       EditorComponentRegistry.cpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       September 07, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <rttr/registration>
#include <PrefabComponent.hpp>
#include <EditorComponentRegistry.hpp>
namespace Borealis
{
    static std::vector<std::string> GetPropertyNames(std::string componentName)
    {
        std::vector<std::string> propertyNames;
        if (componentName == "Prefab Component")
        {
			propertyNames.push_back("PrefabID");
			propertyNames.push_back("Edited Component List");
			propertyNames.push_back("ParentID");
		}
		return propertyNames;
    }
    static std::vector<std::string> GetComponentNames()
    {
        std::vector<std::string> componentNames;
		componentNames.push_back("Prefab Component");
		return componentNames;
    }

    std::vector<std::string> EditorComponentRegistry::mComponentNames;
    std::vector<std::vector<std::string>> EditorComponentRegistry::mPropertyNames;
    

    using namespace rttr;
    RTTR_REGISTRATION
    {
         registration::class_<PrefabComponent>("Prefab Component")
            (metadata("Component", true))
            .constructor<>()
            .property("PrefabID", &PrefabComponent::mPrefabID)
            .property("Edited Component List", &PrefabComponent::mEditedComponentList)
            .property("ParentID", &PrefabComponent::mParentID);
    }
}

