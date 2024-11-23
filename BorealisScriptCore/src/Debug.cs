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

        public static void DrawLine(Vector3 start, Vector3 end, Color color)
        {
            InternalCalls.DrawLine(start, end, color);
        }

        public static void DrawRay(Vector3 start, Vector3 direction, Color color)
        {
            DrawLine(start, start + direction, color);
        }
    }
  }
