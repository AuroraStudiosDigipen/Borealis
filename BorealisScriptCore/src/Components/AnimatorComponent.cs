
using System;

namespace Borealis
{
    [NativeComponent]
    public class AnimatorComponent : Component
    {
        public AnimatorComponent()
        {
        }
        public AnimatorComponent(ulong id)
        {
            InstanceID = id;
            gameObject = new GameObject(id);

        }
        public Animation currentAnimation
        {
            get
            {
                InternalCalls.AnimatorComponent_GetCurrentAnimation(gameObject.GetInstanceID(), out ulong id);
                Animation animation = new Animation(id);
                return animation;
            }
            set
            {
                if (value is null)
                    InternalCalls.AnimatorComponent_SetCurrentAnimation(gameObject.GetInstanceID(), 0);
                else
                    InternalCalls.AnimatorComponent_SetCurrentAnimation(gameObject.GetInstanceID(), value.InstanceID);
            }
        }

        public Animation nextAnimation
        {
            get
            {
                InternalCalls.AnimatorComponent_GetNextAnimation(gameObject.GetInstanceID(), out ulong id);
                Animation animation = new Animation(id);
                return animation;
            }
            set
            {
                if (value is null)
                    InternalCalls.AnimatorComponent_SetNextAnimation(gameObject.GetInstanceID(), 0);
                else
                    InternalCalls.AnimatorComponent_SetNextAnimation(gameObject.GetInstanceID(), value.InstanceID);
            }
        }

        public float speed
        {
            get
            {
                InternalCalls.AnimatorComponent_GetSpeed(gameObject.GetInstanceID(), out float speed);
                return speed;
            }
            set
            {
                InternalCalls.AnimatorComponent_SetSpeed(gameObject.GetInstanceID(), value);
            }
        }

        public bool loop
        {
            get
            {
                InternalCalls.AnimatorComponent_GetLooping(gameObject.GetInstanceID(), out bool loop);
                return loop;
            }
            set
            {
                InternalCalls.AnimatorComponent_SetLooping(gameObject.GetInstanceID(), value);
            }
        }

        public float blend
        {
            get
            {
                InternalCalls.AnimatorComponent_GetBlend(gameObject.GetInstanceID(), out float blend);
                return blend;
            }
            set
            {
                InternalCalls.AnimatorComponent_SetBlend(gameObject.GetInstanceID(), value);
            }
        }

        public float currentTime
        {
            get
            {
                InternalCalls.AnimatorComponent_GetCurrentTime(gameObject.GetInstanceID(), out float time);
                return time;
            }
        }

        public float duration
        {
            get
            {
                InternalCalls.AnimatorComponent_GetAnimationDuration(gameObject.GetInstanceID(), out float duration);
                return duration;
            }
        }


        public void SwapBlendBuffer()
        {
            InternalCalls.AnimatorComponent_SwapBlendBuffer(gameObject.GetInstanceID());
        }
    }
           
    

}
