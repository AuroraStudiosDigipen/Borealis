
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

        public void PlayOneShot(AudioClip clip, float volumeScale = 1.0F)
        {
            InternalCalls.AudioSource_PlayOneShot(InstanceID, volumeScale, clip.InstanceID);
        }

        public void Play()
        {
            InternalCalls.AudioSource_Play(InstanceID);
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
