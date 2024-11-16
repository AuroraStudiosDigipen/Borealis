/******************************************************************************
/*!
\file       BehaviourNode.cpp
\author     Joey Chua
\par        email: joeyjunyu.c@digipen.edu
\date       September 15, 2024
\brief      Defines the class for behaviour node

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#include "BorealisPCH.hpp"
#include "AI/BehaviourTree/BehaviourNode.hpp"
#include "AI/BehaviourTree/BehaviourTree.hpp"
#include <Scripting/ScriptingUtils.hpp>
#include <Core/LoggerSystem.hpp>
#include <mono/jit/jit.h>
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

namespace Borealis
{


    /*
        @brief Retrieves the depth of the node in the behavior tree.
        @return The depth as an integer.
    */
    int BehaviourNode::GetDepth() const
    {
        auto klass = GetScriptClassUtils("BehaviourNode");
        auto method = klass->GetMethod("GetDepth", 0);
        auto result = klass->InvokeMethod(mInstance->GetInstance(), method, nullptr);
        return *(int*)mono_object_unbox(result);
    }

    std::string BehaviourNode::GetName() const
    {
        auto klass = GetScriptClassUtils("BehaviourNode");
        auto method = klass->GetMethod("GetName", 0);
        auto result = klass->InvokeMethod(mInstance->GetInstance(), method, nullptr);

        MonoString* mono_str_result = (MonoString*)result;
        const char* c_string_result = mono_string_to_utf8(mono_str_result);
        std::string output(c_string_result);
        mono_free((void*)c_string_result);
        return output;
    }

    /*
        @brief Sets the depth of the node.
        @param depth The depth to be set.
    */
    void BehaviourNode::SetDepth(unsigned int depth) 
    {
        auto klass = GetScriptClassUtils("BehaviourNode");
        auto method = klass->GetMethod("SetDepth", 1);
        void* params[1];
        params[0] = &depth;
        auto result = klass->InvokeMethod(mInstance->GetInstance(), method, params);
    }

    /*
        @brief Adds a child node to the current node.
        @param child The BehaviourNode to add as a child.
    */
    void BehaviourNode::AddChild(BehaviourNode child)
    {
        auto klass = GetScriptClassUtils("BehaviourNode");
        auto method = klass->GetMethod("AddChild", 1);
        void* params[1];
        params[0] = child.mInstance->GetInstance();
        auto result = klass->InvokeMethod(mInstance->GetInstance(), method, params);

    }

    /*
        @brief Checks if the node is currently running.
        @return True if the node is RUNNING, otherwise false.
    */
    bool BehaviourNode::IsRunning() const
    {
        auto klass = GetScriptClassUtils("BehaviourNode");
        auto method = klass->GetMethod("IsRunning", 0);
        auto result = klass->InvokeMethod(mInstance->GetInstance(), method, nullptr);
        return *(bool*)mono_object_unbox(result);
    }
    /*
        @brief Sets the node's status.
        @param newStatus The new status to set.
    */
    void BehaviourNode::SetStatus(NodeStatus newStatus)
    {
        auto klass = GetScriptClassUtils("BehaviourNode");
        auto method = klass->GetMethod("SetStatus", 1);
        void* params[1];
        params[0] = &newStatus;
        auto result = klass->InvokeMethod(mInstance->GetInstance(), method, params);
    }

    /*
        @brief Updates the node based on its current status.
        @param dt The delta time for updating the node.
    */
    void BehaviourNode::Tick(float dt, Entity& entity)
    {
        auto gameObjectKlass = GetScriptClassUtils("GameObject")->GetMonoClass();
        //Instantiate a game object
        auto gameObject = mono_object_new(mono_domain_get(), gameObjectKlass);
        // Run constructor
        {
            auto gameObjectConstructor = mono_class_get_method_from_name(gameObjectKlass, ".ctor", 1);
            void* param[1];
            param[0] = &entity.GetComponent<IDComponent>().ID;
            mono_runtime_invoke(gameObjectConstructor, gameObject, param, nullptr);
        }

        auto klass = GetScriptClassUtils("BehaviourNode");
        auto method = klass->GetMethod("Tick", 2);
        void* params[2];
        params[0] = &dt;
        params[1] = gameObject;
        auto result = klass->InvokeMethod(mInstance->GetInstance(), method, params);
    }

    BehaviourNode BehaviourNode::Clone()
    {
        return BehaviourNode(this->GetName());
    }

    BehaviourNode::BehaviourNode(MonoObject* objectInstance)
    {
        mInstance = MakeRef<ScriptInstance>(objectInstance);
    }

    BehaviourNode::BehaviourNode(std::string klassName)
    {
        mInstance = MakeRef<ScriptInstance>(GetScriptClassUtils(klassName));
        // Set node type, node status, node result, node depth, node name        
        {
            auto klass = GetScriptClassUtils("BehaviourNode");
            auto method = klass->GetMethod("SetResult", 1);
            void* params[1];
            auto r = NodeResult::SUCCESS;
            params[0] = &r;
            klass->InvokeMethod(mInstance->GetInstance(), method, params);
        }
        SetDepth(0);

        {
            auto klass = GetScriptClassUtils("BehaviourNode");
            auto method = klass->GetMethod("SetName", 1);
            void* params[1];
            MonoString* string = mono_string_new(mono_domain_get(), klassName.c_str());
            params[0] = string;
            klass->InvokeMethod(mInstance->GetInstance(), method, params);
        }
        auto klass = mInstance->GetMonoClass();
        MonoCustomAttrInfo* attributeInfo = mono_custom_attrs_from_class(klass);

        if (attributeInfo)
        {
            auto attributeClass = mono_custom_attrs_get_attr(attributeInfo, GetScriptAttribute("BTNodeClass"));
            if (attributeClass)
            {

                auto field = mono_class_get_field_from_name(mono_object_get_class(attributeClass), "nodeType");
                int nodeType;
                mono_field_get_value(attributeClass, field, &nodeType);
                
                auto Scriptklass = GetScriptClassUtils("BehaviourNode");
                auto method = Scriptklass->GetMethod("SetNodeType", 1);
                void* params[1];
                params[0] = &nodeType;
                Scriptklass->InvokeMethod(mInstance->GetInstance(), method, params);

            }
        }
    }

    std::vector<BehaviourNode> BehaviourNode::GetChildrenNodes() const
    {

        std::vector<BehaviourNode> output;
        auto klass = GetScriptClassUtils("BehaviourNode");
        auto method = klass->GetMethod("GetChildrenNodesCPP", 0);
        auto result = klass->InvokeMethod(mInstance->GetInstance(), method, nullptr);

        MonoArray* monoArray = (MonoArray*)result;
        int length = mono_array_length(monoArray);

        for (int i = 0; i < length; ++i) {
            // Retrieve the element at index 'i'
            MonoObject* elementObj = mono_array_get(monoArray, MonoObject*, i);
            output.push_back(elementObj);
;
        }


        return output;
    }
}
