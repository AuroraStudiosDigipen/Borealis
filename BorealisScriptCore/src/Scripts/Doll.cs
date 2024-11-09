using Borealis;

public class Doll : MonoBehaviour
{
    GameObject Player;
    Inventory inventory;
    float distanceCap;

    public override void Update()
    {
        if (Vector3.Distance(Player.transform.position, gameObject.transform.position) < distanceCap)
        {
            inventory.GrabItem(ItemType.Doll);
            gameObject.SetActive(false);
            return;
        }
    }
}

