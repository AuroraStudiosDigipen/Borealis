using System;
using System.Linq;

namespace Borealis
{
    [BTNodeClass(NodeType.DECORATOR)]
    public class RepeatNodeUnlimited : BehaviourNode
    {
        uint counter;
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
            child.Tick(dt, gameobject);
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
                SetStatusChildren(NodeStatus.READY);
                SetResultChildren(NodeResult.SUCCESS);
                OnFailure();
            }
        }

        protected override void OnExit()
        {
            // Any cleanup if needed
            Debug.Log("Stopping sound ");

            base.OnExit();
        }
    }
}
