
namespace Borealis
{
    public class CharacterController : Component
    {
        //public int childCount;
        public CharacterController(ulong id)
        {
            InstanceID = id;
        }

        public void Move(Vector3 motion)
        {
            InternalCalls.CharacterController_Move(GetInstanceID(), ref motion);
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
