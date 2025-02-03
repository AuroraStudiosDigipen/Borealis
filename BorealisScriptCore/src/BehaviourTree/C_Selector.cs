using System;
using System.Collections.Generic;

namespace Borealis
{
    [BTNodeClass(NodeType.CONTROLFLOW)]

    public class Selector : BehaviourNode
    {
        private int currentIndex;

        public Selector()
        {
            currentIndex = 0;
        }

        protected override void OnEnter()
        {
            currentIndex = 0;
            base.OnEnter();
        }

        protected override void OnUpdate(float dt, GameObject gameobject)
        {
            // if any child succeeds, node succeeds
            // if all children fail, node fails
            BehaviourNode currentNode = mChildren[currentIndex];
            //Debug.Log("Selector " + currentNode.GetName());
            currentNode.Tick(dt, gameobject);

            if (currentNode.HasSucceeded() == true)
            {
                OnSuccess();
            }
            else if (currentNode.HasFailed() == true)
            {
                // move to the next node
                ++currentIndex;

                // if we hit size, that means all nodes failed
                if (currentIndex == mChildren.Count)
                {
                    OnFailure();
                }
            }
        }

        protected override void OnExit()
        {
            // Any cleanup if needed
            base.OnExit();
        }
    }
}
