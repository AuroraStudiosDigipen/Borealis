
namespace Borealis
{
    [NativeComponent]
    public class AudioListener : Component
    {
        public AudioListener() { }
        public AudioListener(ulong id)
        {
            InstanceID = id;
            gameObject = new GameObject(id);
        }

        public void SetListener()
        {
            InternalCalls.AudioListener_SetListener(InstanceID);
        }
    }
}
