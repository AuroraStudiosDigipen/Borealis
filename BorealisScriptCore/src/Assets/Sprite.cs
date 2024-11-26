
namespace Borealis
{
    [AssetField(AssetType.Sprite)]
    public class Sprite : Object
    {
        public Sprite(ulong id)
        {
            InstanceID = id;
        }

        public Sprite()
        {
            InstanceID = 0;
        }
    }
}
