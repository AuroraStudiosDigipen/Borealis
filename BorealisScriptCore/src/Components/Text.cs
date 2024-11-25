
namespace Borealis
{
    public class TextComponent : Component
    {
        public TextComponent(ulong id)
        {
            InstanceID = id;
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
                InternalCalls.TextComponent_SetText(GetInstanceID(), ref value);
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
