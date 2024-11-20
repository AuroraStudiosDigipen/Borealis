using System;

namespace Borealis
{
    [BTNodeClass(NodeType.LEAF)]
    public class Idle : BehaviourNode
    {
        private float timer;

        public Idle()
        {
        }

        protected override void OnEnter()
        {
            timer = 3.0f;
            OnLeafEnter();
        }

        protected override void OnUpdate(float dt, GameObject gameobject)
        {
            timer -= dt;

            if (timer <= 0.0f)
            {
                Debug.Log("Idling completed for GameObject" + gameobject.name);
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
