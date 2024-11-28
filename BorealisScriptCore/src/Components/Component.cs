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
        public Transform transform
        {
            get
            {
                return new Transform(InstanceID);
            }
        }
    }
}
