namespace Borealis
{
    public class Input
    {
        public static bool anyKey
        {
            get
            {
                InternalCalls.Input_AnyKey(out bool key);
                return key;
            }
        }

        public static bool anyKeyDown
        {
            get
            {
                InternalCalls.Input_AnyKeyDown(out bool key);
                return key;
            }
        }

        public static Vector3 mousePosition
        {
            get
            {
                return InternalCalls.Input_GetMousePosition();
            }
        }

        public static Vector2 mouseScrollDelta
        {
            get
            {
                return InternalCalls.Input_GetMouseScrollDelta();
            }
        }

        public static float GetAxis(string axisName)
        {
            return InternalCalls.Input_GetAxis(axisName);
        }
        public static float GetAxisRaw(string axisName)
        {
            return InternalCalls.Input_GetAxisRaw(axisName);
        }

        public static bool GetKey(KeyCode key)
        {
            return InternalCalls.Input_GetKey((int)key);
        }

        public static bool GetKeyDown(KeyCode key)
        {
            return InternalCalls.Input_GetKeyDown((int)key);
        }
        public static bool GetKeyUp(KeyCode key)
        {
            return InternalCalls.Input_GetKeyUp((int)key);
        }

        public static bool GetMouse(KeyCode mouse)
        {
            return InternalCalls.Input_GetMouse((int)mouse);
        }

        public static bool GetMouseDown(KeyCode mouse)
        {
            return InternalCalls.Input_GetMouseDown((int)mouse);
        }
        public static bool GetMouseUp(KeyCode mouse)
        {
            return InternalCalls.Input_GetMouseUp((int)mouse);
        }
    }
}
