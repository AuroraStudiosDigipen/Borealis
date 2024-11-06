
using Microsoft.CodeAnalysis.Diagnostics;
using System;
using System.Collections.Generic;

namespace Borealis
{
    public class GameObject : Object
    {
        //public bool activeSelf => true; // Use internal call
        public Transform transform;
        public GameObject()
        {
            InstanceID = InternalCalls.CreateEntity("untitled");
            transform = new Transform(InstanceID);

        }
        internal GameObject(ulong id)
        {
            InstanceID = id;
            transform = new Transform(id);
        }

        public GameObject(string name)
        {
            InstanceID = InternalCalls.CreateEntity(name);
            transform = new Transform(InstanceID);
        }

        
        public T AddComponent<T>() where T : Component, new()
        {
            if (HasComponent<T>())
            {
                return GetComponent<T>(); // Log that the component already exists
            }
            else
            {
                Type componentType = typeof(T);
                if (componentType.IsSubclassOf(typeof(MonoBehaviour)))
                {
                    InternalCalls.ScriptComponent_AddComponent(GetInstanceID(), typeof(T));
                    T component = new T { gameObject = this };
                    component.InstanceID = GetInstanceID();
                    return component;
                }
                else
                { 
                    InternalCalls.Entity_AddComponent(GetInstanceID(), typeof(T));
                    T component = new T { gameObject = this };
                    component.InstanceID = GetInstanceID();
                    return component;
                }
            }
        }

        public bool HasComponent<T>() where T : Component
        {
            if (typeof(T).IsSubclassOf(typeof(MonoBehaviour)))
            {
                return InternalCalls.ScriptComponent_HasComponent(GetInstanceID(), typeof(T));
            }
            return InternalCalls.Entity_HasComponent(GetInstanceID(), typeof(T));
        }
        public bool HasComponent(Type type)
        {
            if (type.IsSubclassOf(typeof(MonoBehaviour)))
            {
                return InternalCalls.ScriptComponent_HasComponent(GetInstanceID(), type);
            }
            return InternalCalls.Entity_HasComponent(GetInstanceID(), type);
        }
        public void RemoveComponent<T>() where T : Component
        {
            if (typeof(T).IsSubclassOf(typeof(MonoBehaviour)))
            {
                InternalCalls.ScriptComponent_RemoveComponent(GetInstanceID(), typeof(T));
            }
            InternalCalls.Entity_RemoveComponent(GetInstanceID(), typeof(T));
        }
        public T GetComponent<T>() where T : Component, new()
        {
            if (HasComponent<T>() || HasComponent(typeof(T)))
            {
                T component = new T { gameObject = this };
                return component;
            }
            else
            {
               InternalCalls.Log(name + " does not have a " + typeof(T).Name + " component.");
                T component = new T { gameObject = this };
                return component;
            }
        }
        public void SetActive(bool value) 
        { 
            InternalCalls.SetActive(value, out InstanceID);
        }

    }
}
