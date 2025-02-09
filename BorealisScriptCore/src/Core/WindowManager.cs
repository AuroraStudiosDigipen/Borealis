
namespace Borealis
{
    public class WindowManager
    {
        public static void SetFullScreen( bool bFullScreen )
        {
            InternalCalls.SetFullscreen(bFullScreen);
        }
    }
}
