using System;
using System.Collections.Generic;
namespace Borealis
{
    [BTNodeClass(NodeType.LEAF)]
    public class Patrol : BehaviourNode
    {
        public List<Transform> wayPoints = new List<Transform> { /*waypoint cordinates*/};
        public float distance;
        public Transform transform;

        public Patrol()
        {
            distance = 0;
            transform = null;  
            
        }

        protected override void OnEnter()
        {
            OnLeafEnter();
        }

        protected override void OnUpdate(float dt, GameObject gameobject)
        {
            foreach (Transform t in wayPoints)
            {
                gameobject.GetComponent<Rigidbody>().position = t.position;
            }

            if (true)
            {
                Debug.Log(gameobject.name);
                OnSuccess();
            }
        }

        protected override void OnExit()
        {
            // Any cleanup if needed
            base.OnExit();
        }
    }
}
