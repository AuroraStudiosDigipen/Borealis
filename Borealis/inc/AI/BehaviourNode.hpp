#pragma once
#include <string>
#include <memory>
#include <vector>
enum class NodeType
{
    CONTROLFLOW,
    DECORATOR,
    LEAF,
    UNKNOWN
};

enum class NodeStatus
{
    READY, // node is enterable
    RUNNING, // node is currently running
    EXITING, // node has succeeded or failed
    SUSPENDED // node won't exceute anything
};

enum class NodeResult
{
    IN_PROGRESS, // still being run 
    SUCCESS, // node succeeded
    FAILURE // node failed
};

class  BehaviourNode
{
    friend class BehaviorTreePrototype;
public:
    BehaviourNode(NodeType type, int depth, const std::string& name);
    ~BehaviourNode();

    // Getters for type, depth, and name
    NodeType get_type() const;
    int get_depth() const;
    std::string get_name() const;

    // Node management
    void add_child(BehaviourNode* child);

    // readability status getters
    bool is_ready() const;
    bool succeeded() const;
    bool failed() const;
    bool is_running() const;
    bool is_suspended() const;

    // set just this node's status
    void set_status(NodeStatus newStatus);

    // set this node and all childrens' status, recursively
    void set_status_all(NodeStatus newStatus);

    // set only the direct children's status
    void set_status_children(NodeStatus newStatus);

    void set_result(NodeResult result);

    void set_result_children(NodeResult result);

    NodeStatus get_status() const;

    NodeResult get_result() const;

    void tick(float dt);

    //const char* get_name() const;
    //const char* get_summary() const;

    virtual BehaviourNode* clone() = 0;

protected:
    //BehaviorAgent* agent;
    NodeType nodeType;
    NodeStatus status;
    NodeResult result;
    BehaviourNode* parent;
    std::vector<BehaviourNode*> children;
    std::string name;  // Name of the node

    void on_leaf_enter();

    // override for any non-generic logic
    virtual void on_enter();
    virtual void on_update(float dt);
    virtual void on_exit();

    // convenience functions for setting status and result
    void on_success();
    void on_failure();

private:
    int mDepth;

};

// just a simple CRTP intermediary for cloning derived types
template <typename T>
class BaseNode : public BehaviourNode
{
public:
    virtual BehaviourNode* clone()
    {
        T& self = *static_cast<T*>(this);
        return new T(self);
    }
};