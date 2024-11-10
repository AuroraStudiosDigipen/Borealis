namespace Borealis
{
    public class BrittleBricks : MonoBehaviour
    {
        public GameObject player;
        public GameObject Gap;
        private int maxBrittleCount = 2;
        private int currentBrittleCount = 0;
        private bool isParented = false;
        private bool placed = false;
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
                        Vector3 Scale = gameObject.transform.localScale;

                        gameObject.transform.parent = player;
                        gameObject.transform.position = new Vector3( 0, 70f, 0);
                        gameObject.transform.localScale = new Vector3(0.29f, 0.29f, 0.29f);
                        gameObject.transform.localRotation = new Vector3(0, 0, 0);
                    }
                }
                else
                {
                    if (Vector3.Distance(player.transform.position, Gap.transform.position) < 2 && !placed)
                    {
                        placed = true;
                        // Set parent here
                        gameObject.transform.parent = Gap;
                        gameObject.transform.position = new Vector3(0, 0, 0);
                        gameObject.transform.localScale = new Vector3(1, 1, 1);
                        gameObject.transform.localRotation = new Vector3(0, 0, 0);
                    }
                }
            }
        }

        private void OnCollisionEnter(GameObject obj)
        {
            if (obj.GetInstanceID() == player.GetInstanceID())
            {
                //if (collision.contacts[0].normal == Vector3.down)
                //{
                //    HandleBrittleWithDelay(0.1f);  // Add a 0.1s delay before brittle
                //}
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
