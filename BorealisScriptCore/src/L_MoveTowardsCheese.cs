using System;

namespace Borealis
{
    [BTNodeClass(NodeType.LEAF)]
    public class L_MoveTowardsCheeseAction : BehaviourNode
    {
        private Vector3 closestCheesePosition;   // Position of the cheese
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
            closestCheesePosition = GetBlackboard().GetValue<Vector3>("closestCheesePosition");
            // Calculate the direction towards the cheese
            Vector3 direction = (closestCheesePosition - currentPosition).normalized;

            // Move the enemy towards the cheese             //motion -> Direction(normalized) * speed
            gameobject.GetComponent<CharacterController>().Move(direction * moveSpeed);

            Debug.Log($"Moving towards cheese. Current position: {currentPosition}, Target position: {closestCheesePosition}");

            // Check if the enemy has reached the cheese
            float distanceToCheese = Vector3.Distance(gameobject.GetComponent<Transform>().position, closestCheesePosition);
            if (distanceToCheese <= 0.1f) // A small threshold to consider it "reached"
            {
                Debug.Log("Reached the cheese.");
                OnSuccess();
            }
        }

        protected override void OnExit()
        {
            base.OnExit();
        }
    }
}
