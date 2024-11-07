

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
                InternalCalls.RigidbodyComponent_GetVelocity(GetInstanceID(), out Vector3 velocity);
                return velocity;
            }
            set
            {
                InternalCalls.RigidbodyComponent_SetVelocity(GetInstanceID(), ref value);
            }
        }


        public float mass
        {
            get
            {
                InternalCalls.RigidbodyComponent_GetMass(GetInstanceID(), out float mass);
                return mass;
            }
            set
            {
                InternalCalls.RigidbodyComponent_SetMass(GetInstanceID(), ref value);
            }
        }

        public bool useGravity
        {
            get
            {
                InternalCalls.RigidbodyComponent_GetUseGravity(GetInstanceID(), out bool gravity);
                return gravity;
            }
            set
            {
                InternalCalls.RigidbodyComponent_SetUseGravity(GetInstanceID(), ref value);
            }
        }

        public bool isKinematic
        {
            get
            {
                InternalCalls.RigidbodyComponent_GetIsKinematic(GetInstanceID(), out bool gravity);
                return gravity;
            }
            set
            {
                InternalCalls.RigidbodyComponent_SetIsKinematic(GetInstanceID(), ref value);
            }
        }

        public void AddForce(Vector3 force, ForceMode mode = ForceMode.Force)
        {

        }

        public void AddForceAtPosition(Vector3 force, Vector3 position, ForceMode mode = ForceMode.Force)
        {

        }

        public void AddRelativeForce(Vector3 force, ForceMode mode = ForceMode.Force)
        {

        }

        public void AddRelativeTorque(Vector3 torque, ForceMode mode = ForceMode.Force)
        {

        }

        public void AddTorque(Vector3 torque, ForceMode mode = ForceMode.Force)
        {

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
