
using Microsoft.CodeAnalysis.Diagnostics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace Borealis
{
    public class GameObject : Object
    {
        //public bool activeSelf => true; // Use internal call

        public new string name
        {
            get
            {
                InternalCalls.Entity_GetName(InstanceID , out string name);
                return name;
            }
            set
            {
                InternalCalls.Entity_SetName(InstanceID, value);
            }
        }

        public string tag
        {
            get
            {
                InternalCalls.Entity_GetTag(InstanceID, out string name);
                return name;
            }
            set
            {
                InternalCalls.Entity_SetTag(InstanceID, value);
            }
        }
        public Transform transform
        {
            get
            {
                return new Transform(InstanceID);
            }
            set
            {
                transform = value;
            }
        }
        static public GameObject Find(string name)
        {
            InternalCalls.Entity_FindEntity(name, out ulong ID);
            return new GameObject(ID);
        }
        public bool activeSelf
        {
            get
            {
                InternalCalls.Entity_GetActive(GetInstanceID(), out bool active);
                return active;
            }
            set
            {
                InternalCalls.Entity_SetActive(GetInstanceID(), ref value);
            }
        }
        public GameObject()
        {
            InstanceID = InternalCalls.CreateEntity("untitled");
            transform = new Transform(InstanceID);

        }
        public GameObject(ulong id)
        {
            InstanceID = id;
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
                if (typeof(T).IsDefined(typeof(NativeComponent), false))
                {
                    return new T { InstanceID = GetInstanceID(), gameObject = this };
                }
                InternalCalls.Entity_GetComponent(InstanceID, typeof(T), out object component);
                return (T)component;
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

        public static GameObject[] FindObjectsWithLayerMask(int mask)
        {
            InternalCalls.Entity_GetEntitiesFromLayer(mask, out GameObject[] entities);
            return entities;
        }

        public static GameObject[] FindGameObjectsWithTag(string tag)
        {
            InternalCalls.Entity_GetEntitiesFromTag(tag, out GameObject[] entities);
            return entities;
        }
    }
}
