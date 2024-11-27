using System;

namespace Borealis
{
    [BTNodeClass(NodeType.LEAF)]
    public class L_MoveTowardsCheeseAction : BehaviourNode
    {
        private Vector3 cheesePosition;   // Position of the cheese
        private float moveSpeed = 5;          // Movement speed of the enemy

        public L_MoveTowardsCheeseAction()
        {
        }
        
        protected override void OnEnter()
        {
            // Find the cheese
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

            // Get the current position of the enemy
            Vector3 currentPosition = gameobject.GetComponent<Transform>().position;

            // Calculate the direction towards the cheese
            //Vector3 direction = gameobject.GetComponent<Transform>().forward;
            Vector3 direction = (cheesePosition - currentPosition).normalized;

            // Move the enemy towards the cheese             //motion -> Direction(normalized) * speed
            gameobject.GetComponent<CharacterController>().Move(direction * moveSpeed);

            Debug.Log($"Moving towards cheese. Current position: {currentPosition}, Target position: {cheesePosition}");

            // Check if the enemy has reached the cheese
            float distanceToCheese = Vector3.Distance(gameobject.GetComponent<Transform>().position, cheesePosition);
            if (distanceToCheese <= 0.1f) // A small threshold to consider it "reached"
            {
                Debug.Log("Reached the cheese.");
                OnSuccess();
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
