

namespace Borealis
{
    [NativeComponent]
    public class MeshRenderer : Component
    {
        public MeshRenderer()
        {
        }
        public MeshRenderer(ulong id)
        {
            InstanceID = id;
            gameObject = new GameObject(id);

        }
        public Material material
        {
            get
            {
                InternalCalls.MeshRendererComponent_GetMaterial(gameObject.GetInstanceID(), out ulong id);
                Material material = new Material(id);
                return material;
            }
            set
            {
                if (value is null)
                {
                    ulong zero = 0;
                    InternalCalls.MeshRendererComponent_SetMaterial(gameObject.GetInstanceID(), ref zero);
                }
                else
                    InternalCalls.MeshRendererComponent_SetMaterial(gameObject.GetInstanceID(), ref value.InstanceID);
            }
        }

        public bool enabled
        {
            get
            {
                InternalCalls.MeshRendererComponent_GetEnabled(gameObject.GetInstanceID(), out bool state);
                return state;
            }
            set
            {
                InternalCalls.MeshRendererComponent_SetEnabled(gameObject.GetInstanceID(), ref value);
            }
        }
    }
}
