
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

        public void PlayOneShot(AudioClip clip, float volumeScale = 1.0F, bool is2D = false, float minDist = 0f, float maxDist = 100F)
        {
            InternalCalls.AudioSource_PlayOneShot(InstanceID, volumeScale, clip.InstanceID, is2D, minDist, maxDist);
        }

        public void PlayOneShotPosition(AudioClip clip, Vector3 position, float volumeScale = 1.0F, float minDist = 0f, float maxDist = 100F)
        {
            InternalCalls.AudioSource_PlayOneShotPosition(InstanceID , volumeScale, clip.InstanceID, ref position, minDist, maxDist);
        }

        public void Play()
        {
            InternalCalls.AudioSource_Play(InstanceID);
        }

        public void Stop()
        {
            InternalCalls.AudioSource_Stop(InstanceID);
        }

        public AudioClip clip
        {
            get
            {
                InternalCalls.AudioSource_GetClip(GetInstanceID(), out ulong clipID);
                return new AudioClip(clipID);
            }
            set
            {
                InternalCalls.AudioSource_SetClip(GetInstanceID(), ref value.InstanceID);
            }
        }

        public bool isPlaying
        {
            get
            {
                InternalCalls.AudioSource_IsPlaying(GetInstanceID(), out bool playing);
                return playing;
            }
        }

        public bool loop
        {
            get
            {
                InternalCalls.AudioSource_GetLooping(GetInstanceID(), out bool looping);
                return looping;
            }
            set
            {
                InternalCalls.AudioSource_SetLooping(GetInstanceID(), ref value);
            }
        }

        public float volume
        {
            get
            {
                InternalCalls.AudioSource_GetVolume(GetInstanceID(), out float volume);
                return volume;
            }
            set
            {
                InternalCalls.AudioSource_SetVolume(GetInstanceID(), ref value);
            }
        }
    }
}
