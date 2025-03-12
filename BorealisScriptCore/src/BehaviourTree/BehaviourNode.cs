
using System;
using System.Collections.Generic;

namespace Borealis
{
    public class Blackboard
    {
        private Dictionary<string, IBlackboardValue> _data = new Dictionary<string, IBlackboardValue>();
        public Blackboard() { }
        public void SetValue<T>(string key, T value)
        {
            _data[key] = new BlackboardValue<T>(value);
        }

        public T GetValue<T>(string key)
        {
            return _data.TryGetValue(key, out var value) && value is BlackboardValue<T> typedValue ? typedValue.Value : default;
        }

        private interface IBlackboardValue { }
        private class BlackboardValue<T> : IBlackboardValue
        {
            public T Value { get; }
            public BlackboardValue(T value) { Value = value; }
        }
    }
    public enum NodeType
    {
        CONTROLFLOW,
        DECORATOR,
        LEAF,
        ROOT,
        UNKNOWN
    };

    public enum  NodeStatus
    {
        READY, // node is enterable
        RUNNING, // node is currently running
        EXITING, // node has succeeded or failed
        SUSPENDED // node won't exceute anything
    };

    public enum  NodeResult
    {
        IN_PROGRESS, // still being run 
        SUCCESS, // node succeeded
        FAILURE // node failed
    };

    public class BehaviourNode
    {
        private void InitBlackboard() {
            Console.WriteLine("Blackboard Created");
            blackboard = new Blackboard(); 
        }
        public List<BehaviourNode> GetChildrenNodes()
        {
            return mChildren;
        }

        private BehaviourNode[] GetChildrenNodesCPP()
        {
            return mChildren.ToArray();
        }

        public NodeType GetNodeType()
        {
            return mNodeType;
        }
        private void SetNodeType(NodeType type)
        {
            mNodeType = type;
        }
        public void SetDepth(uint depth)
        {
            mDepth = depth;
        }

        public uint GetDepth()
        {
            return mDepth;
        }
        public string GetName()
        {
            return mName;
        }

        public BehaviourNode GetParent()
        {
            return mParent;
        }
        public void SetName(string setName)
        {
            mName = setName;
        }
        public void AddChild(BehaviourNode child)
        {
            mChildren.Add(child);
            child.blackboard = this.blackboard;
            child.mParent = this;
        }

        public bool IsReady()
        {
            return mStatus == NodeStatus.READY;
        }

        public bool HasSucceeded()
        {
            return mResult == NodeResult.SUCCESS;
        }

        public bool HasFailed()
        {
            return mResult == NodeResult.FAILURE;
        }
        public bool IsRunning()
        {
            return mStatus == NodeStatus.RUNNING;
        }

        public bool IsSuspended()
        {
            return mStatus == NodeStatus.SUSPENDED;
        }

        public void SetStatus(NodeStatus newStatus)
        {
            mStatus = newStatus;
        }

        public void SetStatusAll(NodeStatus newStatus)
        {
            mStatus = newStatus;
            for (int i = 0; i < mChildren.Count; i++)
            {
                mChildren[i].SetStatusAll(newStatus);
            }
        }


        public void SetStatusChildren(NodeStatus newStatus)
        {
            for (int i = 0; i < mChildren.Count; i++)
            {
                mChildren[i].SetStatusAll(newStatus);
            }
        }

        public void SetResult(NodeResult result)
        {
            mResult = result;
        }

        public void SetResultChildren(NodeResult result)
        {
            for (int i = 0; i < mChildren.Count; i++)
            {
                mChildren[i].SetResult(result);
            }
        }

        public NodeStatus GetStatus()
        {
            return mStatus;
        }

        public NodeResult GetResult() 
        {
            return mResult;
        }

        public void Tick(float dt, GameObject gameobject)
        {
            if (mStatus == NodeStatus.READY)
            {
                OnEnter();
            }

            if (mStatus == NodeStatus.RUNNING)
            {
                OnUpdate(dt, gameobject);
            }

            if (mStatus == NodeStatus.EXITING)
            {
                OnExit();
            }
        }
        public Blackboard GetBlackboard()
        {
            return this.blackboard;
        }

        //TBD
        protected BehaviourNode Clone()
        {
            return new BehaviourNode();
        }

        protected void OnLeafEnter()
        {
            SetStatus(NodeStatus.RUNNING);
            SetResult(NodeResult.IN_PROGRESS);
        }

        protected virtual void OnEnter()
        {
            // Base logic is to mark as running
            SetStatus(NodeStatus.RUNNING);
            SetResult(NodeResult.IN_PROGRESS);

            // And this node's children as ready to run
            SetStatusChildren(NodeStatus.READY);
            SetResultChildren(NodeResult.IN_PROGRESS);
        }
        protected virtual void OnUpdate(float dt, GameObject gameobject) { }

        protected virtual void OnExit() { }

        protected void OnSuccess() 
        {
            SetStatus(NodeStatus.EXITING);
            SetResult(NodeResult.SUCCESS);
        }

        protected void OnFailure()
        {
            SetStatus(NodeStatus.EXITING);
            SetResult(NodeResult.FAILURE);
        }
        protected void OnRunning()
        {
            // If the node is not already running, switch it to RUNNING.
            if (mStatus != NodeStatus.RUNNING)
            {
                SetStatus(NodeStatus.RUNNING);
            }

            // If the node's result is not already IN_PROGRESS, set it now.
            if (mResult != NodeResult.IN_PROGRESS)
            {
                mResult = NodeResult.IN_PROGRESS;
            }
        }


        //protected BehaviourNode mParent;  // Parent node reference
        public BehaviourNode mParent { get; private set; }
        protected List<BehaviourNode> mChildren = new List<BehaviourNode>();

        private Blackboard blackboard = new Blackboard();
        //// Node type, status, and result
        private uint mDepth;  // Depth of the node in the behavior tree
        private NodeType mNodeType;
        private NodeStatus mStatus;
        private NodeResult mResult;
        private string mName;  // Name of the node
    };
}
