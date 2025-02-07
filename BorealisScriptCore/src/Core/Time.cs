
namespace Borealis
{
    public class Time
    {
        public static float deltaTime 
        {
            get
            {
                return InternalCalls.Time_GetDeltaTime();
            }
        }

        public static float unscaledDeltaTime
        {
            get
            {
                return InternalCalls.Time_GetUnscaledDeltaTime();
            }
        }

        public static float timeScale
        {
            get
            {
                return InternalCalls.Time_GetTimeScale();
            }
            set
            {

                InternalCalls.Time_SetTimeScale(value);
            }
        }


        public static float fixedTime { get;}
    }
}
