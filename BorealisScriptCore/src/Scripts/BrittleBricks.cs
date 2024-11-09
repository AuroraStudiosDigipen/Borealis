namespace Borealis
{
    public class BrittleBricks : MonoBehaviour
    {
        public GameObject player;
        public GameObject Gap;
        private int maxBrittleCount = 2;
        private int currentBrittleCount = 0;
        bool isParented = false;
        // Delay to prevent instant destruction

        public override void Update()
        {
            if (Input.GetKeyDown(KeyCode.E))
            {
                // by distance
                if (!isParented)
                { 
                    if (Vector3.Distance(player.transform.position, gameObject.transform.position) < 2)
                    {
                        isParented = true;
                        // Set parent here
                        gameObject.transform.position = new Vector3( 0, 5f, 0);
                    }
                }
                else
                {
                    if (Vector3.Distance(player.transform.position, Gap.transform.position) < 2)
                    {
                        isParented = false;
                        // Set parent here
                        gameObject.transform.position = new Vector3(0, 0, 0);
                    }
                }
            }
        }

        private void OnCollisionEnter(GameObject obj)
        {
            if (obj.GetInstanceID() == player.GetInstanceID())
            {
                if (collision.contacts[0].normal == Vector3.down)
                {
                    HandleBrittleWithDelay(0.1f);  // Add a 0.1s delay before brittle
                }
            }
        }

        private void HandleBrittleWithDelay(float delay)
        {

            // Increase the brittle count
            currentBrittleCount++;

            if (currentBrittleCount == 1)
            {
                // Change texture
            }
            else if (currentBrittleCount >= maxBrittleCount)
            {
                // Use MoodSwitcher to remove the object from the spawned lists
                gameObject.SetActive(false);  // Second jump destroys the cube
            }

        }
    }
}
