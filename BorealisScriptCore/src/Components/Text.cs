
namespace Borealis
{
    [NativeComponent]
    public class Text : Component
    {
        public Text() { }
        public Text(ulong id)
        {
            InstanceID = id;
            gameObject = new GameObject(id);

        }


        public string text
        {
            get
            {
                InternalCalls.TextComponent_GetText(GetInstanceID(), out string text);
                return text;
            }
            set
            {
                InternalCalls.TextComponent_SetText(GetInstanceID(), value);
            }
        }

        public Color color
        {
            get
            {
                InternalCalls.TextComponent_GetColor(GetInstanceID(), out Color color);
                return color;
            }
            set
            {
                InternalCalls.TextComponent_SetColor(GetInstanceID(), ref value);
            }
        }

    }
}
