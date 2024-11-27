using System;
using System.Collections.Generic;
namespace Borealis
{
    [BTNodeClass(NodeType.LEAF)]
    public class Patrol : BehaviourNode
    {
        public List<Vector3> patrolPoints = new List<Vector3> { /*waypoint cordinates*/};
        public float distance;
        public Transform transform;
        int currentPointIndex;
        float moveSpeed = 5;
        public Patrol()
        {
        }

        protected override void OnEnter()
        {
            //get all waypoint entitys
            //add all entity pos to waypoint list
            // Find the cheese
            // auto cheeseEntities = GameObject.GetEntitiesByLayer(string);
            // for (auto entity: cheeseEntities)
            // {
            //entity.transform <distance>
            //patrolPoints.Add(transform);

            //}
            OnLeafEnter();
        }

        protected override void OnUpdate(float dt, GameObject gameobject)
        {
            if (patrolPoints.Count ==0)
            {
                OnFailure();
                return;
            }

            Vector3 targetPoint = patrolPoints[currentPointIndex];
            // Calculate direction to target
            Vector3 agentPosition = gameobject.GetComponent<Transform>().position;
            Vector3 direction = targetPoint - agentPosition;
            Vector3.Normalize(direction);
            //motion -> Direction(normalized) * speed
            gameobject.GetComponent<CharacterController>().Move(direction * moveSpeed);
            float distance = Vector3.Distance(targetPoint,gameobject.GetComponent<Transform>().position);
            if (distance < 0.1)
            {
                currentPointIndex = (currentPointIndex + 1) % patrolPoints.Count;
                OnSuccess();
            }
            else
            {
                OnFailure();
            }
        }

        protected override void OnExit()
        {
            // Any cleanup if needed
            base.OnExit();
        }
    }
}
