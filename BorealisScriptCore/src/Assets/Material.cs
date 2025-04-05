
namespace Borealis
{
    [AssetField(AssetType.Material)]
    public class Material : Object
    {
        public Material(ulong ID)
        {
            InstanceID = ID;
        }
        public Material()
        {
            InstanceID = 0;
        }

        public Sprite mainTexture
        {
            get
            {
                InternalCalls.Material_GetSprite(InstanceID, out ulong spriteID);
                return new Sprite(spriteID);
            }
            set
            {
                if (value is null)
                {
                    ulong zero = 0;
                    InternalCalls.Material_SetSprite(InstanceID, ref zero);
                }
                else
                    InternalCalls.Material_SetSprite(InstanceID, ref value.InstanceID);
            }
        }

        public Vector2 tiling
        {
            get
            {
                InternalCalls.Material_GetTiling(InstanceID, out Vector2 Tiling);
                return new Vector2(Tiling.x, Tiling.y);
            }
            set
            {
                InternalCalls.Material_SetTiling(InstanceID, ref value);
            }
        }


        public Vector2 offset
        {
            get
            {
                InternalCalls.Material_GetOffset(InstanceID, out Vector2 Offset);
                return new Vector2(Offset.x, Offset.y);
            }
            set
            {
                InternalCalls.Material_SetOffset(InstanceID, ref value);
            }
        }

    }
}
