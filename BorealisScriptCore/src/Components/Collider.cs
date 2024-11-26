

namespace Borealis
{
    public struct Bounds
    {
        public Vector3 center;
        public Vector3 extents;
        public Vector3 max;
        public Vector3 min;
        public Vector3 size;
    }
    [NativeComponent]
    public class Collider : Component
    {
        public Collider() { }
        public Collider(ulong id)
        {
            InstanceID = id;
            gameObject = new GameObject(id);

        }


        public bool CompareTag(string tag)
        {
            return gameObject.name == tag;
        }

        public Bounds bounds
        {
            get
            {
                InternalCalls.ColliderComponent_GetBounds(GetInstanceID(), out Vector3 center, out Vector3 extents, out Vector3 max, out Vector3 min, out Vector3 size);
                Bounds bounds = new Bounds();
                bounds.center = center;
                bounds.extents = extents;
                bounds.max = max;
                bounds.min = min;
                bounds.size = size;
                return bounds;
            }
        }
    }
}
