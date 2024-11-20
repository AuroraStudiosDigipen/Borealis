using System;
using System.Collections.Generic;

namespace Borealis
{
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
            if (currentIndex >= mChildren.Count)
            {
                OnSuccess();
                return;
            }

            BehaviourNode currentNode = mChildren[currentIndex];
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
