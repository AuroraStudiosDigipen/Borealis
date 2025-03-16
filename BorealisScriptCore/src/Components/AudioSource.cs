
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

        public void PlayOneShot(AudioClip clip)
        {
            InternalCalls.AudioSource_PlayOneShot(InstanceID, clip.audioName);
        }

        public void PlayOneShotPosition( AudioClip clip, Vector3 position)
        {
            InternalCalls.AudioSource_PlayOneShotPosition(InstanceID, clip.audioName, ref position);
        }
        public void Stop()
        {
            InternalCalls.AudioSource_Stop(InstanceID);
        }

        public bool isPlaying
        {
            get
            {
                InternalCalls.AudioSource_IsPlaying(GetInstanceID(), out bool playing);
                return playing;
            }
        }
    }
}
