

namespace Borealis
{
    public enum ForceMode
    { 
        Force,
        Acceleration,
        Impulse,
        VelocityChange
    }

    public class Rigidbody : Component
    {
        //public int childCount;
        internal Rigidbody(ulong id)
        {
            InstanceID = id;
        }
        public Vector3 linearVelocity
        {
            get
            {
                InternalCalls.RigidbodyComponent_GetLinearVelocity(GetInstanceID(), out Vector3 velocity);
                return velocity;
            }
            set
            {
                InternalCalls.RigidbodyComponent_SetLinearVelocity(GetInstanceID(), ref value);
            }
        }

        public Vector3 angularVelocity
        {
            get
            {
                InternalCalls.RigidbodyComponent_GetAngularVelocity(GetInstanceID(), out Vector3 velocity);
                return velocity;
            }
            set
            {
                InternalCalls.RigidbodyComponent_SetAngularVelocity(GetInstanceID(), ref value);
            }
        }

        public Vector3 position
        {
            get
            {
                InternalCalls.RigidbodyComponent_GetPosition(GetInstanceID(), out Vector3 position);
                return position;
            }
            set
            {
                InternalCalls.RigidbodyComponent_SetPosition(GetInstanceID(), ref value);
            }
        }

        public Vector3 rotation
        {
            get
            {
                InternalCalls.RigidbodyComponent_GetRotation(GetInstanceID(), out Vector3 rotation);
                return rotation;
            }
            set
            {
                InternalCalls.RigidbodyComponent_SetRotation(GetInstanceID(), ref value);
            }
        }


        //public float mass
        //{
        //    get
        //    {
        //        InternalCalls.RigidbodyComponent_GetMass(GetInstanceID(), out float mass);
        //        return mass;
        //    }
        //    set
        //    {
        //        InternalCalls.RigidbodyComponent_SetMass(GetInstanceID(), ref value);
        //    }
        //}

        //public bool useGravity
        //{
        //    get
        //    {
        //        InternalCalls.RigidbodyComponent_GetUseGravity(GetInstanceID(), out bool gravity);
        //        return gravity;
        //    }
        //    set
        //    {
        //        InternalCalls.RigidbodyComponent_SetUseGravity(GetInstanceID(), ref value);
        //    }
        //}

        //public bool isKinematic
        //{
        //    get
        //    {
        //        InternalCalls.RigidbodyComponent_GetIsKinematic(GetInstanceID(), out bool gravity);
        //        return gravity;
        //    }
        //    set
        //    {
        //        InternalCalls.RigidbodyComponent_SetIsKinematic(GetInstanceID(), ref value);
        //    }
        //}

        public void AddForce(Vector3 force, ForceMode mode = ForceMode.Force)
        {
            InternalCalls.RigidbodyComponent_AddForce(GetInstanceID(), ref force);
        }

        public void AddImpulse(Vector3 force, ForceMode mode = ForceMode.Force)
        {
            InternalCalls.RigidbodyComponent_AddImpulse(GetInstanceID(), ref force);
        }
        public void AddTorque(Vector3 torque, ForceMode mode = ForceMode.Force)
        {
            InternalCalls.RigidbodyComponent_AddTorque(GetInstanceID(), ref torque);
        }

        public void Move(Vector3 position, Quaternion rotation)
        {

        }

        public void MovePosition(Vector3 position)
        {

        }

        public void MoveRotation(Quaternion rot)
        {

        }
    }
}
