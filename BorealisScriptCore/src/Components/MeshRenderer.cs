

namespace Borealis
{
    public class MeshRenderer : Component
    {
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
                InternalCalls.MeshRendererComponent_SetMaterial(gameObject.GetInstanceID(), ref value.InstanceID);
            }
        }
    }
}
