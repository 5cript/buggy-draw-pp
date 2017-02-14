#include "node.hpp"

namespace BuggyDraw
{
//#####################################################################################################################
    bool isAtomic(Node const& node)
    {
        if (node.atomic)
            return node.atomic.get();
        return false;
    }
//---------------------------------------------------------------------------------------------------------------------
    bool containsCompound(Node const& node)
    {
        if (!node.nodes)
            return false;

        for (auto const& node : node.nodes.get())
        {
            if (!isAtomic(node))
                return true;
        }
        return false;
    }
//---------------------------------------------------------------------------------------------------------------------
    Node* getNodeById(Node* node, std::string const& id)
    {
        if (node->id == id)
            return node;

        if (!node->nodes)
            return nullptr;

        for (auto& n : node->nodes.get())
        {
            if (isAtomic(n) && n.id == id)
                return &n;
            else
            {
                auto* res = getNodeById(&n, id);
                if (res != nullptr)
                    return res;
            }
        }
        return nullptr;
    }
//---------------------------------------------------------------------------------------------------------------------
    int countInputs(Node const& node)
    {
        int count = 0;
        for (auto const& port : node.ports)
        {
            if (port.kind == "input")
                ++count;
        }
        return count;
    }
//---------------------------------------------------------------------------------------------------------------------
    void forAllNodes(Node& baseNode, std::function <void(Node&)> const& modifier)
    {
        modifier(baseNode);
        if (!baseNode.nodes)
            return;
        for (auto& node : baseNode.nodes.get())
            forAllNodes(node, modifier);
    }
//---------------------------------------------------------------------------------------------------------------------
    void initializeDrawInformation(Node& node)
    {
        forAllNodes(node, [](Node& n){
            n.drawInformation = DrawInformation{};
        });
    }
//#####################################################################################################################
}
