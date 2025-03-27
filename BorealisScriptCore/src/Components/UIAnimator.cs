
using System;

namespace Borealis
{
    [NativeComponent]
    public class UIAnimator : Component
    {
        public bool isPlaying
        {
            get
            {
                InternalCalls.UIAnimator_GetPlaying(gameObject.GetInstanceID(), out bool isPlaying);
                return isPlaying;
            }
            set
            {
                InternalCalls.UIAnimator_SetPlaying(gameObject.GetInstanceID(), value);
            }
        }

        public bool endLoop
        {
            get
            {
                InternalCalls.UIAnimator_GetEndLoop(gameObject.GetInstanceID(), out bool endLoop);
                return endLoop;
            }
        }
    }
}
