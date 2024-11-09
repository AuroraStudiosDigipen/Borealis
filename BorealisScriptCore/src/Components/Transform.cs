

namespace Borealis
{
    public class Transform : Component
    {
        //public int childCount;
        internal Transform(ulong id)
        {
            InstanceID = id;
        }
        public Vector3 localRotation
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

        //public Quaternion localRotation
        //{
        //    get
        //    {

        //    }
        //    set
        //    {

        //    }
        //}

        public Vector3 localScale
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

        public Vector3 forward
        {
            get
            {
                Quaternion rotation = Quaternion.Euler(localRotation.x, localRotation.y, localRotation.z);
                return rotation * Vector3.forward;
            }
            set
            {
                Quaternion rotation = new Quaternion();
                rotation.SetLookRotation(value);
                localRotation = rotation.eulerAngles;
            }
        }

        public Vector3 right
        {
            get
            {
                Quaternion rotation = Quaternion.Euler(localRotation.x, localRotation.y, localRotation.z);
                return rotation * Vector3.right;
            }
        }

        public Vector3 up
        {
            get
            {
                Quaternion rotation = Quaternion.Euler(localRotation.x, localRotation.y, localRotation.z);
                return rotation * Vector3.up;
            }
        }
    }
}
