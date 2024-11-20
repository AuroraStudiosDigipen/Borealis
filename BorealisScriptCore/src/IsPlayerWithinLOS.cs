using System;

namespace Borealis
{
    public class IsPlayerWithinLOS : BehaviourNode
    {
        private Vector3 targetPosition;   // Position of the target (e.g., the player)
        private float maxViewDistance;    // Maximum distance the enemy can "see"
        private int layerMask;            // Layer mask to detect the player

        // Constructor to initialize the node
        public IsPlayerWithinLOS()
        {
            layerMask = 1; // Default set to 1 for player layer
        }

        protected override void OnEnter()
        {
            // Called when the condition starts
            OnLeafEnter();
            InternalCalls.Log("Checking line of sight...");
        }

        protected override void OnUpdate(float dt, GameObject gameobject)
        {
            // Get the enemy's position (as the GameObject references the enemy)
            Vector3 enemyPosition = gameobject.GetComponent<Rigidbody>().position;

            // Create a ray from the enemy's position towards the target
            Ray ray = new Ray(enemyPosition, (targetPosition - enemyPosition).normalized);

            // Perform a raycast using the Physics library
            RaycastHit[] hits = Physics.RaycastAll(ray, maxViewDistance, layerMask);

            // Check if any of the hits are directly on the target
            bool hasLineOfSight = false;

            foreach (var hit in hits)
            {
                if (hit.Transform != null && hit.Transform.position == targetPosition)
                {
                    hasLineOfSight = true;
                    break;
                }
            }

            if (hasLineOfSight)
            {
                InternalCalls.Log("Target is within line of sight.");
                OnSuccess(); // LOS condition passed
            }
            else
            {
                InternalCalls.Log("Target is not within line of sight.");
                OnFailure(); // LOS condition failed
            }
        }

        protected override void OnExit()
        {
            // Cleanup if necessary
            InternalCalls.Log("Exiting IsPlayerWithinLOS.");
            base.OnExit();
        }
    }
}
