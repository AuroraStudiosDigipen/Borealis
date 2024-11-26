
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
                InternalCalls.Material_SetSprite(InstanceID, ref value.InstanceID);
            }
        }
    }
}
