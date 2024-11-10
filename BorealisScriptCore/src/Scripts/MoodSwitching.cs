
using System;


namespace Borealis
{
    public class MoodSwitching : MonoBehaviour
    {
        public bool Happiness = false;
        public GameObject happinessObjects;
        public GameObject sadnessObjects;
        public float range;
        public GameObject player;
        public override void Start()
        {
        }

        public override void Update()
        {
            if (Input.GetKeyDown(KeyCode.E))
            {
                if (Vector3.Distance(player.transform.position, gameObject.transform.position) < range)
                {
                    Debug.Log("Works");

                    Happiness = !Happiness;
                    if (Happiness)
                    {
                        sadnessObjects.SetActive(true);
                        happinessObjects.SetActive(false);
                    }
                    else
                    {
                        sadnessObjects.SetActive(false);
                        happinessObjects.SetActive(true);
                    }
                }
            }
        }

    }
}
