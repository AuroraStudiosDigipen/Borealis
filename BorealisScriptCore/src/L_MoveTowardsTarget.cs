using System;

namespace Borealis
{
    [BTNodeClass(NodeType.LEAF)]
    public class L_MoveTowardsTarget : BehaviourNode
    {
        private Vector3 targetPosition;   // Position of the cheese
        private float moveSpeed = 5;          // Movement speed of the enemy
        GameObject playerEntity;
        GameObject closestCheeseEntity;
        public L_MoveTowardsTarget()
        {
        }
        
        protected override void OnEnter()
        {
            // Find the player and
            // auto cheeseEntities = GameObject.GetEntitiesByLayer(string);
            // for (auto entity: cheeseEntities)
            // {
            //entity.transform <distance>
            //}
            OnLeafEnter();
            Debug.Log("Starting to move towards cheese.");
        }

        protected override void OnUpdate(float dt, GameObject gameobject)
        {

            float stoppingDistance = 0;
            // Check priorities: player first, then cheese
            //if (gameobject.cheeseDetected)
            if (true)
            {
                // find nearest cheese and set target pos to found pos
                targetPosition = closestCheeseEntity.GetComponent<Transform>().position;
            }
            //else if (gameobject.playerDetected)
            else if (true)
            {
                targetPosition = playerEntity.GetComponent<Transform>().position;
            }
            else
            {
                // No valid target; stop movement
                OnFailure(); // Fail this node
                return;
            }

            // Calculate direction to target
            Vector3 agentPosition = gameobject.GetComponent<Transform>().position;
            Vector3 direction = targetPosition - agentPosition;
            float distance = direction.magnitude;
            //motion -> Direction(normalized) * speed
            gameobject.GetComponent<CharacterController>().Move(direction * moveSpeed);

            // Stop moving if within the stopping distance
            if (distance <= stoppingDistance)
            {
                //set enemy velocity to 0
                OnSuccess(); // Task is complete
                return;
            }
        }

        protected override void OnExit()
        {
            // Any cleanup logic
            Debug.Log("Exiting L_MoveTowardsCheeseAction.");
            base.OnExit();
        }
    }
}
