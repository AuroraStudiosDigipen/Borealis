
namespace Borealis
{
    [NativeComponent]
    public class Canvas : Component
    {
        public Canvas() { }
        public Canvas(ulong id)
        {
            InstanceID = id;
            gameObject = new GameObject(id);
        }

        public float alpha
        {
            get
            {
                InternalCalls.Canvas_GetAlpha(GetInstanceID(), out float alphaValue);
                return alphaValue;
            }
            set
            {
                InternalCalls.Canvas_SetAlpha(GetInstanceID(), ref value);
            }
        }
    }
}
