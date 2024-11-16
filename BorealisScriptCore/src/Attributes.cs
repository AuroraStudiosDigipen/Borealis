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

    public class BTNodeClass : Attribute
    {
        public NodeType nodeType;

        public BTNodeClass(NodeType nodeType)
        {
            this.nodeType = nodeType;
        }
    }
}
