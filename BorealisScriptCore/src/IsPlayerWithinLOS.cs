using System;

namespace Borealis
{
    public class IsPlayerWithinLOS : BehaviourNode
    {
        private Vector3 playerPosition;        // Position of the target (e.g., the player)
        private float maxViewDistance = 50;    // Maximum distance the enemy can "see"
        private float viewAngle = 135;         // Maximum angle the enemy can "see"
        private int layerMask;                 // Layer mask to detect the player
        GameObject playerEntity;
        // Constructor to initialize the node
        public IsPlayerWithinLOS()
        {
            layerMask = 1; // Default set to 1 for player layer
        }

        protected override void OnEnter()
        {
            // Find the player
            // playerEntity = GameObject.GetEntitiesByLayer(string);
            
            OnLeafEnter();
        }

        protected override void OnUpdate(float dt, GameObject gameobject)
        {
            playerPosition = playerEntity.GetComponent<Transform>().position;
            // Calculate direction to the player
            Vector3 directionToPlayer = playerPosition - gameobject.GetComponent<Transform>().position;
            float distance = directionToPlayer.magnitude;

            // Check if the player is beyond the maximum view distance
            if (distance > maxViewDistance)
            {
                //set bool for player seen to false
                //playerEntity.isSeen = false;
                OnFailure();
                return;
            }

            // Normalize the direction vector
            Vector3.Normalize(directionToPlayer);

            // Get and normalize the agent's forward vector
            Vector3 agentForward = gameobject.GetComponent<Transform>().forward;
            Vector3.Normalize(agentForward);

            // Calculate the dot product between the forward vector and direction to player
            float dotProduct = Vector3.Dot(agentForward,directionToPlayer);

            // Clamp the dot product to the range [-1, 1] to prevent acosf issues
            dotProduct = Math.Max(-1.0f, Math.Min(1.0f, dotProduct));

            // Calculate the angle between the forward vector and the direction to the player
            double angleToPlayer = Math.Acos((double)dotProduct);
            angleToPlayer = angleToPlayer * (180.0 / 3.14159265);
            // Check if the player is within the field of view (half the view angle)
            if (angleToPlayer <= (viewAngle / 2.0f))
            {
                //std::cout << "Player is within LOS. Angle: " << angleToPlayer << std::endl;

                // Update the player bool
                //playerEntity.isSeen = true;
                // Signal success
                OnSuccess();
            }
            else
            {
                // std::cout << "Player is outside LOS. Angle: " << angleToPlayer << std::endl;

                // Update the blackboard to indicate no LOS
                //playerEntity.isSeen = false;

                // Signal failure
                OnFailure();
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
