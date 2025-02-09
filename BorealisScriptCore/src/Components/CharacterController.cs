
namespace Borealis
{
    [NativeComponent]
    public class CharacterController : Component
    {
        //public int childCount;
        public CharacterController()
        {
        }
        public CharacterController(ulong id)
        {
            InstanceID = id;
            gameObject = new GameObject(id);

        }

        public void Move(Vector3 motion)
        {
            InternalCalls.CharacterController_Move(GetInstanceID(), ref motion);
        }
        public void Jump(float speed)
        {
            InternalCalls.CharacterController_Jump(GetInstanceID(), ref speed);
        }

        public Vector3 linearVelocity
        {
            get
            {
                InternalCalls.CharacterController_GetLinearVelocity(GetInstanceID(), out Vector3 vel);
                return vel;
            }
            set
            {
                InternalCalls.CharacterController_SetLinearVelocity(GetInstanceID(), ref value);
            }
        }

        public bool isGrounded
        {
            get
            {
                InternalCalls.CharacterController_IsGrounded(GetInstanceID(), out bool grounded);
                return grounded;
            }
        }
    }
}
