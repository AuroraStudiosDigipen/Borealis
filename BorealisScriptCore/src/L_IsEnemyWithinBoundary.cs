using System;
using Borealis;

namespace Borealis
{
    [BTNodeClass(NodeType.LEAF)]
    public class L_IsEnemyWithinBoundary : BehaviourNode
    {
        private Vector3 boundaryMin;      // The minimum corner of the boundary box
        private Vector3 boundaryMax;      // The maximum corner of the boundary box
        private Transform enemyTransform; // The enemy's transform

        // Constructor to initialize the node
        public L_IsEnemyWithinBoundary()
        {
        }

        protected override void OnEnter()
        {
            // Called when the condition starts
            OnLeafEnter();
            Debug.Log("Checking if enemy is within boundary...");
        }

        protected override void OnUpdate(float dt, GameObject gameobject)
        {
            if (enemyTransform == null)
            {
                Debug.Log("Enemy transform is null. Failing boundary check.");
                OnFailure();
                return;
            }

            // Get the current position of the enemy
            Vector3 enemyPosition = enemyTransform.position;

            // Check if the enemy is within the boundary box
            bool isWithinBoundary = enemyPosition.x >= boundaryMin.x && enemyPosition.x <= boundaryMax.x &&
                                    enemyPosition.y >= boundaryMin.y && enemyPosition.y <= boundaryMax.y &&
                                    enemyPosition.z >= boundaryMin.z && enemyPosition.z <= boundaryMax.z;

            if (isWithinBoundary)
            {
                Debug.Log("Enemy is within the boundary.");
                OnSuccess(); // Boundary condition passed
            }
            else
            {
                Debug.Log("Enemy is outside the boundary.");
                OnFailure(); // Boundary condition failed
            }
        }

        protected override void OnExit()
        {
            // Cleanup if necessary
            Debug.Log("Exiting L_IsEnemyWithinBoundary.");
            base.OnExit();
        }
    }
}
