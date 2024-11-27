using System;
using System.Linq;

namespace Borealis
{
    [BTNodeClass(NodeType..DECORATOR)]
    public class RepeatNodeUnlimited : BehaviourNode
    {
        unsigned counter;

        public RepeatNodeUnlimited()
        {
        }

        protected override void OnEnter()
        {
            counter = 0;
            OnLeafEnter();
        }

        protected override void OnUpdate(float dt, GameObject gameobject)
        {
            BehaviourNode child = mChildren.FirstOrDefault();
            if (child.HasSucceeded() == true)
            {
                if (counter == 4)// counter does not increment hence infinite repeat
                {
                    OnSuccess();
                }
                else
                {
                    child.SetStatus(NodeStatus.READY);
                }
            }
            else if (child.HasFailed() == true)
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
