namespace Borealis
{
    public class Debug
    {
        public static void Log(object text)
        {
            InternalCalls.Log(text.ToString());
        }

        public static void LogWarning(object text)
        {
            InternalCalls.LogWarning(text.ToString());
        }

        public static void LogError(object text)
        {
            InternalCalls.LogError(text.ToString());
        }
    }
  }
