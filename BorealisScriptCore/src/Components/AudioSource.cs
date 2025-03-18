
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

        public int PlayOneShot(AudioClip clip)
        {
            return InternalCalls.AudioSource_PlayOneShot(InstanceID, clip.audioName);
        }

        public int PlayOneShotPosition( AudioClip clip, Vector3 position)
        {
            return InternalCalls.AudioSource_PlayOneShotPosition(InstanceID, clip.audioName, ref position);
        }
        public void Stop()
        {
            InternalCalls.AudioSource_Stop(InstanceID);
        }

        public void Stop(int ID)
        {
            InternalCalls.AudioSource_StopID(InstanceID, ID);
        }

        public bool isPlaying
        {
            get
            {
                InternalCalls.AudioSource_IsPlaying(GetInstanceID(), out bool playing);
                return playing;
            }
        }

        public bool IsSoundPlaying(int ID)
        {
            return InternalCalls.AudioSource_IsChannelPlaying(GetInstanceID(), ID);
        }
    }
}
