using System;
namespace Borealis
{
    public class SerializeField : Attribute
    {
    }
    public class HideInInspector : Attribute
    {
    }
    public class Header : Attribute
    {
        public string header;
        public Header(string header)
        {
            this.header = header;
        }
    }

    internal class NativeComponent : Attribute { }

    public enum AssetType : int
    {
        Sprite = 0,
        Material = 1,
        Audio = 2,
        Animation = 3
    }
    internal class AssetField : Attribute 
    {

        public int Type;

        public AssetField(AssetType type)
        {
            Type = ((int)type);
        }
    }
    public class BTNodeClass : Attribute
    {
        public NodeType nodeType;

        public BTNodeClass(NodeType nodeType)
        {
            this.nodeType = nodeType;
        }
    }
}
