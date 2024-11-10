
using Borealis;

public class TreasureBox : MonoBehaviour
{
    GameObject Player;
    GameObject Cupboard;
    Inventory inventory;
    float distanceCap;

    public override void Update()
    {
        if (!Cupboard.activeSelf)
           { 
            if (Vector3.Distance(Player.transform.position, gameObject.transform.position) < distanceCap)
            {
                if (Input.GetKeyDown(KeyCode.E))
                {
                    if (inventory.hasKey())
                    {
                        inventory.UseKey();
                        gameObject.SetActive(false);
                        return;
                    }
                }
            }
        }
    }
}

