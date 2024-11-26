
namespace Borealis
{
    [NativeComponent]
    public class AudioSource : Component
    {
        public AudioSource() { }
        public AudioSource(ulong id)
        {
            InstanceID = id;
            gameObject = new GameObject(id);

        }
    }
}
