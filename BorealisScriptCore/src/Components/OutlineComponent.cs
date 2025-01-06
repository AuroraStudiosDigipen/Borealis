

namespace Borealis
{
    [NativeComponent]
    public class OutLine : Component
    {
        public OutLine()
        {
        }
        public OutLine(ulong id)
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
