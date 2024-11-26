

namespace Borealis
{
    [NativeComponent]
    public class SpriteRenderer : Component
    {
        //public int childCount;
        public SpriteRenderer()
        {
        }
        public SpriteRenderer(ulong id)
        {
            InstanceID = id;
            gameObject = new GameObject(id);

        }

        public Color color
        {
            get
            {
                InternalCalls.SpriteRendererComponent_GetColor(gameObject.GetInstanceID(), out Color color);
                return color;
            }
            set
            {
                InternalCalls.SpriteRendererComponent_SetColor(gameObject.GetInstanceID(), ref value);
            }
        }

        public Sprite sprite
        {
            get
            {
                InternalCalls.SpriteRendererComponent_GetSprite(gameObject.GetInstanceID(), out ulong sprite);
                return new Sprite(sprite);
            }
            set
            {
                InternalCalls.SpriteRendererComponent_SetSprite(gameObject.GetInstanceID(), ref value.InstanceID);
            }
        }
    }
}
