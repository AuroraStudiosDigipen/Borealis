
namespace Borealis
{
    public abstract class State
    {
        protected StateMachine stateMachine;

        public State(StateMachine stateMachine)
        {
            this.stateMachine = stateMachine;
        }

        public abstract void OnEnter();
        public abstract void Update();
        public abstract void OnExit();
    }
}
