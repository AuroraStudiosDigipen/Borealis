
using Microsoft.CodeAnalysis.CSharp.Syntax;
using System.Reflection.Emit;

namespace Borealis
{
    public class LayerMask
    {
        public int value;

        public LayerMask(int value)
        {
            this.value = value;
        }

        public static int GetMask(params string[] layerNames)
        {
            InternalCalls.LayerMask_GetMask(out int mask, layerNames);
            return mask;
        }

        public static string LayerToName(int layer)
        {
            InternalCalls.LayerMask_LayerToName(layer, out string name);
            return name;
        }
        public static int NameToLayer(string layerName)
        {
            InternalCalls.LayerMask_NameToLayer(layerName, out int id);
            return id;
        }

        public static implicit operator int(LayerMask mask)
        {
            return mask.value;
        }
    }
}
