
namespace Borealis
{

    public enum ItemType : int
    {
        None,
        Balloon,
        Key,
        Doll
    }
    public class Inventory : MonoBehaviour

    {
        public int[] slots = new int[3];

        public bool hasKey()
        {
            for (int i = 0; i < slots.Length; i++)
            {
                if (slots[i] == (int)ItemType.Key)
                {
                    return true;
                }
            }
            return false;
        }

        public int numBalloons()
        {
            int count = 0;
            for (int i = 0; i < slots.Length; i++)
            {
                if (slots[i] == (int)ItemType.Balloon)
                {
                    count++;
                }
            }
            return count;
        }

        public bool GrabItem(ItemType item)
        {
            for (int i = 0; i < slots.Length; i++)
            {
                if (slots[i] == (int)ItemType.None)
                {
                    slots[i] = (int)item;
                    return true;
                }
            }
            return false;
        }

        public void UseBalloon()
        {
            if (numBalloons() == 2)
            {
                for (int i = 0; i < slots.Length; i++)
                {
                    if (slots[i] == (int)ItemType.Balloon)
                    {
                        slots[i] = (int)ItemType.None;
                    }
                }
            }
        }

        public bool HasDoll()
        {
            for (int i = 0; i < slots.Length; i++)
            {
                if (slots[i] == (int)ItemType.Doll)
                {
                    return true;
                }
            }
            return false;
        }

        public void UseKey()
        {
            if (hasKey())
            {
                for (int i = 0; i < slots.Length; i++)
                {
                    if (slots[i] == (int)ItemType.Key)
                    {
                        slots[i] = (int)ItemType.None;
                    }
                }
            }
        }

        public void UseDoll()
        {
            if (HasDoll())
            {
                for (int i = 0; i < slots.Length; i++)
                {
                    if (slots[i] == (int)ItemType.Doll)
                    {
                        slots[i] = (int)ItemType.None;
                    }
                }
            }
        }
    }
}
    
