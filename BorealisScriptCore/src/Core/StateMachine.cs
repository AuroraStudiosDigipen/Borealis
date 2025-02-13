using System.Collections.Generic;
namespace Borealis
{
    public class StateMachine
    {
        Dictionary<string, State> states = new Dictionary<string, State>();
        private State currentState;
        public void ChangeState(State newState)
        {
            if (currentState != null)
            {
                currentState.OnExit();
            }

            currentState = newState;
            currentState.OnEnter();
        }

        public void ChangeState(string stateName)
        {
            if (states.ContainsKey(stateName))
            {
                ChangeState(states[stateName]);
            }
        }


        public T GetState<T>(string stateName) where T : State
        {
            return (T)states[stateName];
        }

        public void AddState(string stateName, State state)
        {
            states[stateName] = state;
        }

        public void OnUpdate()
        {
            currentState?.Update();
        }
    }
}