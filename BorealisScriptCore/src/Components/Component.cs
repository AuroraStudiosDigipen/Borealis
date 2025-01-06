namespace Borealis
{
    public class Component : Object
    {
        public GameObject gameObject {
            get
            {
                return new GameObject(InstanceID);
            }
            set
            {
                InstanceID = value.InstanceID;
            }
        }

        public new string name
        {
            get
            {
                InternalCalls.Entity_GetName(InstanceID, out string name);
                return name;
            }
            set
            {
                InternalCalls.Entity_SetName(InstanceID, value);
            }
        }
        public Transform transform
        {
            get
            {
                return new Transform(InstanceID);
            }
        }
    }
}
