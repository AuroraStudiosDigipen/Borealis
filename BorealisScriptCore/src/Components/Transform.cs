

using System.IO;

namespace Borealis
{
    [NativeComponent]
    public class Transform : Component
    {
        //public int childCount;
        public Transform()
        {
        }
        public Transform(ulong id)
        {
            InstanceID = id;
            gameObject = new GameObject(id);

        }

        public Vector3 rotation
        {
            get
            {
                InternalCalls.TransformComponent_GetRotation(GetInstanceID(), out Vector3 rotation);
                return rotation;
            }
            set
            {
                InternalCalls.TransformComponent_SetRotation(GetInstanceID(), ref value);
            }
        }
        public Vector3 localRotation
        {
            get
            {
                InternalCalls.TransformComponent_GetLocalRotation(GetInstanceID(), out Vector3 rotation);
                return rotation;
            }
            set
            {
                InternalCalls.TransformComponent_SetLocalRotation(GetInstanceID(), ref value);
            }
        }

        public GameObject parent
        {
            get
            {
                InternalCalls.TransformComponent_GetParentID(GetInstanceID(), out ulong parentID);
                return new GameObject(parentID);
            }
            set
            {
                ulong refID = value.GetInstanceID();
                InternalCalls.TransformComponent_SetParentID(GetInstanceID(), ref refID);
            }
        }

        public Vector3 scale
        {
            get
            {
                InternalCalls.TransformComponent_GetScale(GetInstanceID(), out Vector3 scale);
                return scale;
            }
            set
            {
                InternalCalls.TransformComponent_SetScale(GetInstanceID(), ref value);
            }
        }

        public Vector3 localScale
        {
            get
            {
                InternalCalls.TransformComponent_GetLocalScale(GetInstanceID(), out Vector3 scale);
                return scale;
            }
            set
            {
                InternalCalls.TransformComponent_SetLocalScale(GetInstanceID(), ref value);
            }
        }
        public Vector3 position
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(GetInstanceID(), out Vector3 translation);
                return translation;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(GetInstanceID(), ref value);
            }
        }


        public Vector3 localPosition
        {
            get
            {
                InternalCalls.TransformComponent_GetLocalTranslation(GetInstanceID(), out Vector3 translation);
                return translation;
            }
            set
            {
                InternalCalls.TransformComponent_SetLocalTranslation(GetInstanceID(), ref value);
            }

        }

        public Vector3 forward
        {
            get
            {
                Quaternion quat = new Quaternion(rotation);
                return -(quat * Vector3.forward);
            }
            set
            {
                Quaternion quat = new Quaternion();
                quat.SetLookRotation(value);
                rotation = quat.eulerAngles;
            }
        }

        public Vector3 right
        {
            get
            {
                Quaternion quat = new Quaternion(rotation);
                return quat * Vector3.right;
            }
        }

        public Vector3 up
        {
            get
            {
                Quaternion quat = new Quaternion(rotation);
                return quat * Vector3.up;
            }
        }

        public int childCount
        {
            get
            {
                InternalCalls.TransformComponent_GetChildCount(GetInstanceID(), out int count);
                return count;
            }
        }

        public GameObject GetChild(int index)
        {
           InternalCalls.TransformComponent_GetChild(GetInstanceID(), index, out ulong childID);
            return new GameObject(childID);
        }

    }
}
