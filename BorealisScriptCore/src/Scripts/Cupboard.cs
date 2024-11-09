
using Borealis;

public class Cupboard : MonoBehaviour
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
                if (inventory.numBalloons() == 2)
                {
                    inventory.UseBalloon();
                    gameObject.SetActive(false);
                    return;
                }
            }
        }
    }
}

