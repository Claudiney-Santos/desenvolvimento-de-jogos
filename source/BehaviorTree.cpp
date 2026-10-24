#include "BehaviorTree.hpp"

SelectorNode::SelectorNode(std::vector<BTNodePtr> children)
    : m_children(std::move(children)) {}

[[nodiscard]] BTResult SelectorNode::tick(const BTContext& context) const {
    for (const auto& child : m_children) {
        BTResult result = child->tick(context);
        if (result.success) {
            return result;
        }
    }
    return {false, ""};
}

SequenceNode::SequenceNode(std::vector<BTNodePtr> children)
    : m_children(std::move(children)) {}

[[nodiscard]] BTResult SequenceNode::tick(const BTContext& context) const {
    std::string lastCommand;
    for (const auto& child : m_children) {
        BTResult result = child->tick(context);
        if (!result.success) {
            return {false, ""};
        }
        if (!result.command.empty()) {
            lastCommand = result.command;
        }
    }
    return {true, lastCommand};
}

ConditionNode::ConditionNode(std::string name)
    : m_name(std::move(name)) {}

[[nodiscard]] BTResult ConditionNode::tick(const BTContext& context) const {
    for (const auto& entry : context) {
        if (entry.first == m_name) {
            return {entry.second != 0, ""};
        }
    }
    return {false, ""};
}

ActionNode::ActionNode(std::string command)
    : m_command(std::move(command)) {}

[[nodiscard]] BTResult ActionNode::tick(const BTContext& context) const {
    (void)context;
    return {true, m_command};
}

BehaviorTree::BehaviorTree(BTNodePtr root)
    : m_root(std::move(root)) {}

[[nodiscard]] std::string BehaviorTree::tick(const BTContext& context) const {
    if (!m_root) {
        return "";
    }
    BTResult result = m_root->tick(context);
    return result.success ? result.command : std::string();
}
