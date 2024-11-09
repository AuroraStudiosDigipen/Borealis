using Borealis;

public class Dollhouse : MonoBehaviour
{
    GameObject Player;
    Inventory inventory;
    float distanceCap;

    public override void Update()
    {
        if (Vector3.Distance(Player.transform.position, gameObject.transform.position) < distanceCap)
        {
            if (Input.GetKeyDown(KeyCode.E))
            {
                if (inventory.HasDoll())
                {
                    inventory.GrabItem(ItemType.Key);
                    inventory.UseDoll();
                    gameObject.SetActive(false);
                    return;
                }
            }
        }
    }
}

