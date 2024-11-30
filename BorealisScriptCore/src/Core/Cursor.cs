namespace Borealis
{
    public enum CursorLockMode
    {
        None,
        Locked,
        Confined
    }
    public class Cursor
    {
        static public bool visible
        {
            get
            {
                InternalCalls.Cursor_GetVisibility(out bool visibility);
                return visibility;
            }
            set
            {
                InternalCalls.Cursor_SetVisibility(ref value);
            }
        }

    }
}
