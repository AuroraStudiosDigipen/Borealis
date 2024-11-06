
using System;


namespace Borealis
{
    public class MoodSwitching : MonoBehaviour
    {
        public bool Happiness = true;
        public GameObject happinessObjects;
        public GameObject sadnessObjects;
        public override void Start()
        {
        }

        public override void Update()
        {
            if (Input.GetKey(KeyCode.E))
            {
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
