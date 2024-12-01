using System;
using System.Collections.Generic;

namespace Borealis
{
    [BTNodeClass(NodeType.CONTROLFLOW)]

    public class Sequencer : BehaviourNode
    {
        private int currentIndex;

        public Sequencer()
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

            BehaviourNode currentNode = mChildren[currentIndex];
            Debug.Log("Sequencer " + currentNode.GetName());
            currentNode.Tick(dt, gameobject);

            if (currentNode.HasFailed())
            {
                OnFailure();
            }
            else if (currentNode.HasSucceeded())
            {
                currentIndex++;
                if (currentIndex >= mChildren.Count)
                {
                    currentIndex = 0;

                    OnSuccess();
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
