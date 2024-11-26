

namespace Borealis
{
    [NativeComponent]
    public class OutlineComponent : Component
    {
        public OutlineComponent()
        {
        }
        public OutlineComponent(ulong id)
        {
            InstanceID = id;
            gameObject = new GameObject(id);
        }
        public bool enabled
        {
            get
            {
                InternalCalls.OutlineComponent_GetEnabled(gameObject.GetInstanceID(), out bool state);
                return state;
            }
            set
            {
                InternalCalls.OutlineComponent_SetEnabled(gameObject.GetInstanceID(), ref value);
            }
        }
    }
}
