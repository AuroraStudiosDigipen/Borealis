

namespace Borealis
{
    [NativeComponent]
    public class CircleRenderer : Component
    {
        //public int childCount;
        public CircleRenderer()
        {
        }
        public CircleRenderer(ulong id)
        {
            InstanceID = id;
            gameObject = new GameObject(id);

        }

        public Color color
        {
            get
            {
                InternalCalls.CircleRendererComponent_GetColor(gameObject.GetInstanceID(), out Color color);
                return color;
            }
            set
            {
                InternalCalls.CircleRendererComponent_SetColor(gameObject.GetInstanceID(), ref value);
            }
        }

        public float thickness
        {
            get
            {
                InternalCalls.CircleRendererComponent_GetThickness(gameObject.GetInstanceID(), out float thickness);
                return thickness;
            }
            set
            {
                InternalCalls.CircleRendererComponent_SetThickness(gameObject.GetInstanceID(), ref value);
            }
        }

        public float fade
        {
            get
            {
                InternalCalls.CircleRendererComponent_GetFade(gameObject.GetInstanceID(), out float fade);
                return thickness;
            }
            set
            {
                InternalCalls.CircleRendererComponent_SetFade(gameObject.GetInstanceID(), ref value);
            }
        }

        
    }
}
