using Borealis;
public class Trampoline : MonoBehaviour
{
    public float bounceForce = 10f;  // How strong the trampoline's bounce is
    public GameObject PlayerParent;
    private void OnCollisionEnter(ulong id)
    {
        GameObject obj = new GameObject(id);
        // Check if the collision object has the tag "Player"
        if (obj.name == "PlayerModel")
        {
            // Get the parent of the PlayerObj (assuming PlayerObj is the collider)
            Transform playerTransform = PlayerParent.transform;
            if (playerTransform != null)
            {
                // Get the Rigidbody component from the parent (Player)
                //Rigidbody rb = playerTransform.GetComponent<Rigidbody>();
                Rigidbody rb = new Rigidbody(GetInstanceID());
                if (rb != null)
                {
                    Vector3 bounce = Vector3.up * bounceForce;
                    rb.AddImpulse(bounce, ForceMode.Impulse); // Impulse mode gives an instant burst of force
                }
            }
        }
    }
}
