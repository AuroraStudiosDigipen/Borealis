
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
        public static float fixedTime { get;}
    }
}
