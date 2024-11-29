using System;
using System.Runtime.CompilerServices;

namespace Borealis
{
    [BTNodeClass(NodeType.LEAF)]
    public class Idle : BehaviourNode
    {
        private float timer;

        public Idle()
        {
            Debug.Log("Idling constructor: " + timer);

        }

        protected override void OnEnter()
        {
            timer = 3.0f;
            Debug.Log("Idling start: " + timer);

            OnLeafEnter();
        }

        protected override void OnUpdate(float dt, GameObject gameobject)
        {
            timer -= dt;
            Debug.Log("Idling time: " + timer);

            GetBlackboard().SetValue("playerisseen", true);
            if (timer <= 0.0f)
            {
                Debug.Log("Idling completed for GameObject" + GetBlackboard().GetValue<bool>("playerisseen"));
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
