using System;

namespace Borealis
{
    [BTNodeClass(NodeType.LEAF)]

    public class IsCheeseWithinRange : BehaviourNode
    {
        //MISSING WAY TO GET CHEESE POSITION
        public Vector3 cheesePosition; // The position of the cheese
        public float detectionRadius = 50;     // Radius within which the cheese is considered "within range"

        public IsCheeseWithinRange()
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
            // Called when the node is first executed
            OnLeafEnter(); // Indicating the node's initial entry
        }

        protected override void OnUpdate(float dt, GameObject gameobject)
        {

            // Calculate the distance between the enemy and the cheese
            float distanceToCheese = Vector3.Distance(gameobject.transform.position, cheesePosition);

            // Check if the distance is within the specified radius
            if (distanceToCheese <= detectionRadius)
            {
                Debug.Log($"Cheese is within radius: {distanceToCheese} <= {detectionRadius}");
                OnSuccess(); // Condition passed
            }
            else
            {
                Debug.Log($"Cheese is outside radius: {distanceToCheese} > {detectionRadius}");
                OnFailure(); // Condition failed
            }

            //motion -> Direction(normalized) * speed
            gameobject.GetComponent<CharacterController>().Move();


        }

        protected override void OnExit()
        {
            // Any cleanup logic, if necessary
            base.OnExit();
        }
    }
}
