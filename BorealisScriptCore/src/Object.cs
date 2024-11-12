using Microsoft.CodeAnalysis.CSharp.Syntax;

namespace Borealis
{
    public class Object
    {

        public string name;
        public ulong GetInstanceID() { return InstanceID; }
        public override string ToString() { return name; }

        public static void Destroy(Object obj, float t = 0.0F)
        {

        }

        public static Object Instantiate(Object original)
        {
            return original;
        }

        public static Object Instantiate(Object original, Transform parent, bool instantiateInWorldSpace)
        {
            return original;
        }

        public static Object Instantiate(Object original, Vector3 position, Quaternion rotation)
        {
            return original;
        }

        public static Object Instantiate(Object original, Vector3 position, Quaternion rotation, Transform parent)
        {
            return original;
        }

        public static bool operator ==(Object x, Object y)
        {
            return x.GetInstanceID() == y.GetInstanceID();
        }

        public static bool operator !=(Object x, Object y) 
        {
            return x.GetInstanceID() != y.GetInstanceID();
        }

        public ulong InstanceID;

        // Operators
        public override bool Equals(object obj)
        {
            return obj is Object other && InstanceID == other.InstanceID;
        }

        public override int GetHashCode()
        {
            return InstanceID.GetHashCode();
        }
    }
}
