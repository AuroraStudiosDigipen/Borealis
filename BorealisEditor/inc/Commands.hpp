/******************************************************************************
/*!
\file       Commands.hpp
\author     Chua Zheng Yang
\par        email: c.zhengyang@digipen.edu
\date       March 18, 2024
\brief      Declares all commands for use in level editor

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef Commands_HPP
#define Commands_HPP
#include <Scene/Entity.hpp>
#include <Scene/ComponentRegistry.hpp>
#include <Scene/SceneManager.hpp>
#include <Scripting/ScriptInstance.hpp>
#include <stack>
#include <memory>
#include <Core/HierarchyLayerManager.hpp>
#include <Scripting/ScriptingUtils.hpp>
#include <Scripting/ScriptingSystem.hpp>
namespace Borealis
{
    // Interface
    class ICommand {
    public:
        virtual ~ICommand() = default;
        virtual void execute() = 0;
        virtual void undo() = 0;
    };

    class GizmoCommand : public ICommand
    {
    private:
        glm::vec3 oldPosition;
        glm::vec3 newPosition;
        glm::vec3 oldScale;
        glm::vec3 newScale;
        glm::vec3 oldRotation;
        glm::vec3 newRotation;
        Entity entity;

    public:
        GizmoCommand(glm::vec3 oldPos, glm::vec3 oldRot, glm::vec3 oldScl, glm::vec3 newPos, glm::vec3 newRot, glm::vec3 newScl, Entity ent)
		: entity(ent), oldPosition(oldPos), newPosition(newPos), oldScale(oldScl), newScale(newScl), oldRotation(oldRot), newRotation(newRot) {}

        void execute() override
        {
			auto& transform = entity.GetComponent<TransformComponent>();
			transform.Translate = newPosition;
			transform.Scale = newScale;
			transform.Rotation = newRotation;
		}

        void undo() override
        {
			auto& transform = entity.GetComponent<TransformComponent>();
			transform.Translate = oldPosition;
			transform.Scale = oldScale;
			transform.Rotation = oldRotation;
		}
    };

    class ModifyNameCommand : public ICommand
    {
    private:
        Entity entity;
        std::string oldValue;
        std::string newValue;
    public:
        ModifyNameCommand(Entity ent, std::string oldVal, std::string newVal)
            : entity(ent), oldValue(oldVal), newValue(newVal) {}

        void execute() override
        {
            entity.GetComponent<TagComponent>().Name = newValue;
        }

        void undo() override
        {
            entity.GetComponent<TagComponent>().Name = oldValue;
        }
    };

    template <typename PropVal>
    class ModifyPropertyCommand : public ICommand {
    private:
        rttr::instance rInstance;
        rttr::property property;
        PropVal oldValue, newValue;

    public:
        ModifyPropertyCommand(rttr::instance instance, rttr::property prop, PropVal oldVal, PropVal newVal)
            : rInstance(instance), property(prop), oldValue(oldVal), newValue(newVal) {}

        void execute() override {
            property.set_value(rInstance, newValue);
        }

        void undo() override {
            property.set_value(rInstance, oldValue);
        }
    };

    template <typename PropVal>
    class ModifyScriptPropertyCommand : public ICommand {
    private:
        Entity entity;
        std::string className;
        PropVal newValue;
        PropVal oldValue;
        std::string name;

    public:
        ModifyScriptPropertyCommand(Entity entt, std::string klass, std::string prop, PropVal oldVal, PropVal newVal)
            : entity(entt), className(klass), name(prop), oldValue(oldVal), newValue(newVal) {}

        void execute() override {
            entity.GetComponent<ScriptComponent>().mScripts[className]->SetFieldValue(name, &newValue);
        }

        void undo() override {
            entity.GetComponent<ScriptComponent>().mScripts[className]->SetFieldValue(name, &oldValue);
        }
    };

    template <typename PropVal>
    class ModifyScriptMonoCommand : public ICommand {
    private:
        Entity entity;
        std::string className;
        PropVal newValue;
        PropVal oldValue;
        std::string name;

    public:
        ModifyScriptMonoCommand(Entity entt, std::string klass, std::string prop, PropVal oldVal, PropVal newVal)
            : entity(entt), className(klass), name(prop), oldValue(oldVal), newValue(newVal) {}

        void execute() override {
            entity.GetComponent<ScriptComponent>().mScripts[className]->SetFieldValue(name, newValue);
        }

        void undo() override {
            entity.GetComponent<ScriptComponent>().mScripts[className]->SetFieldValue(name, oldValue);
        }
    };

    class ModifyScriptAudioCommand : public ICommand
    {
    private:
        ScriptField field;
        MonoObject* data;
        std::string newValue;
        std::string oldValue;

    public:
        ModifyScriptAudioCommand(ScriptField fi, MonoObject* obj, std::string oldVal, std::string newVal)
            : field(fi), data(obj), oldValue(oldVal), newValue(newVal) {}

        void execute() override {
            field.SetAudioName(data, newValue);
        }

        void undo() override {
            field.SetAudioName(data, oldValue);
        }
    };

    class ParentEntityCommand : public ICommand 
    {
    private:
        Entity child;
        UUID oldParent;
		Entity newParent;

        public:
		ParentEntityCommand(Entity child, Entity newParent)
			: child(child), oldParent(child.GetComponent<TransformComponent>().ParentID), newParent(newParent) {}

        void execute() override {
            child.GetComponent<TransformComponent>().ResetParent(child);
			child.GetComponent<TransformComponent>().SetParent(child,newParent);
		}

        void undo() override {
            if (oldParent == 0)
            {
                child.GetComponent<TransformComponent>().ResetParent(child);
                HierarchyLayerManager::GetInstance().LoadEntitiesIntoLayerManager(SceneManager::GetActiveScene());
            }
            else
            {
				child.GetComponent<TransformComponent>().SetParent(child, SceneManager::GetActiveScene()->GetEntityByUUID(oldParent));
			}
        }
    };

    class UnparentEntityCommand : public ICommand
    {
    private:
        Entity child;
        UUID oldParent;

    public:
        UnparentEntityCommand(Entity child)
            : child(child), oldParent(child.GetComponent<TransformComponent>().ParentID){}

        void execute() override {
            child.GetComponent<TransformComponent>().ResetParent(child);
            HierarchyLayerManager::GetInstance().LoadEntitiesIntoLayerManager(SceneManager::GetActiveScene());
        }

        void undo() override {
            if (oldParent!=0)
            {
                child.GetComponent<TransformComponent>().SetParent(child, SceneManager::GetActiveScene()->GetEntityByUUID(oldParent));
            }
        }
    };

    class DestroyEntityCommand : public ICommand
    {
    private:
        Entity entity;
        entt::registry subRegistry;
    public:
        DestroyEntityCommand(Entity ent)
            : entity(ent){}

        void execute() override {
            HierarchyLayerManager::GetInstance().RemoveEntity(entity.GetUUID());
            SceneManager::GetActiveScene()->DestroyEntity(entity);
        }

        void undo() override {
            // Bring back entity
        }
    };


    class DuplicateEntityCommand : public ICommand
    {
    private:
        Entity entity;
        UUID newEntity;
    public:
        DuplicateEntityCommand(Entity ent)
            : entity(ent), newEntity(0) {}

        void execute() override {
                newEntity = SceneManager::GetActiveScene()->DuplicateEntity(entity);
        }

        void undo() override {
            // Bring back entity
            HierarchyLayerManager::GetInstance().RemoveEntity(entity.GetUUID());
            SceneManager::GetActiveScene()->DestroyEntity(SceneManager::GetActiveScene()->GetEntityByUUID(newEntity));

        }
    };

    class CreateEntityCommand : public ICommand
    {
    private:
        Entity newEntity;
        std::string name;
    public:
        CreateEntityCommand(std::string inName)
            : name(inName) {}

        void execute() override {
            newEntity = SceneManager::GetActiveScene()->CreateEntity(name);
        }

        void undo() override {
            // Bring back entity
            HierarchyLayerManager::GetInstance().RemoveEntity(newEntity.GetUUID());
            SceneManager::GetActiveScene()->DestroyEntity(newEntity);

        }
    };

    template <typename ComponentType>
    class AddComponentCommand : public ICommand
    {
    private:
        Entity entity;
    public:
        AddComponentCommand(Entity ent)
            : entity(ent) {}

        void execute() override {
            entity.AddComponent<ComponentType>();
            if (std::is_same<ComponentType, MeshFilterComponent>::value)
            {
                entity.AddComponent<MeshRendererComponent>();
            }
        }

        void undo() override {
            // Bring back entity
            entity.RemoveComponent<ComponentType>();
            if (std::is_same<ComponentType, MeshFilterComponent>::value)
            {
                entity.RemoveComponent<MeshRendererComponent>();
            }
        }
    };

    template <typename ComponentType>
    class RemoveComponentCommand : public ICommand
    {
    private:
        Entity entity;
        ComponentType component;
    public:
        RemoveComponentCommand(Entity ent, ComponentType component)
            : entity(ent), component(component) {}

        void execute() override {
            entity.RemoveComponent<ComponentType>();
        }

        void undo() override {
            // Bring back entity
            entity.AddComponent<ComponentType>(); //Retain all data
            entity.GetComponent<ComponentType>() = component;
        }
    };

    class AddScriptComponentCommand : public ICommand
    {
    private:
        std::string klassName;
        Entity entity;
    public:
        AddScriptComponentCommand(std::string klass, Entity ent)
			: klassName(klass), entity(ent) {}
        void execute() override
        {
            auto klass = GetScriptClassUtils(klassName);
            auto scriptInstance = MakeRef<ScriptInstance>(klass);
            entity.GetComponent<ScriptComponent>().AddScript(klassName, scriptInstance);
            scriptInstance->Init(entity.GetUUID());
            ScriptingSystem::mEntityScriptMap[klassName].insert(entity.GetUUID());

        }

        void undo() override
        {
            ScriptingSystem::mEntityScriptMap[klassName].erase(entity.GetUUID());
			entity.GetComponent<ScriptComponent>().RemoveScript(klassName);
            if (entity.GetComponent<ScriptComponent>().mScripts.empty())
            {
                entity.RemoveComponent<ScriptComponent>();
            }
		}
    };


    class RemoveScriptComponentCommand : public ICommand
    {
    private:
        std::string klassName;
        Entity entity;
    public:
        RemoveScriptComponentCommand(std::string klass, Entity ent)
			: klassName(klass), entity(ent) {}
        void execute() override
        {
            ScriptingSystem::mEntityScriptMap[klassName].erase(entity.GetUUID());
            entity.GetComponent<ScriptComponent>().RemoveScript(klassName);

        }

        void undo() override
        {
            if (!entity.HasComponent<ScriptComponent>()) entity.AddComponent<ScriptComponent>();
            auto klass = GetScriptClassUtils(klassName);
            auto scriptInstance = MakeRef<ScriptInstance>(klass);
            entity.GetComponent<ScriptComponent>().AddScript(klassName, scriptInstance);
            scriptInstance->Init(entity.GetUUID());
            ScriptingSystem::mEntityScriptMap[klassName].insert(entity.GetUUID());

        }
    };


    class ActionManager {
    private:
        static constexpr size_t MAX_HISTORY = 150;

        static void TrimStack(std::stack<std::unique_ptr<ICommand>>& stack) {
            while (stack.size() > MAX_HISTORY) {
                stack.pop();  // Remove the oldest command
            }
        }

        static std::stack<std::unique_ptr<ICommand>> undoStack;
        static std::stack<std::unique_ptr<ICommand>> redoStack;

    public:
        static void execute(std::unique_ptr<ICommand> command);

        static void undo();

        static void redo();
    };
}

#endif
