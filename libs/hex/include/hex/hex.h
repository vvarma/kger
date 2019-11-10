//
// Created by nvr on 23/9/19.
//

#ifndef KGER_HEX_H
#define KGER_HEX_H

#include <map>
#include <vector>
#include <set>

struct HexGraphInternal {
    std::vector<std::set<int>> parentToChild;
    std::vector<std::set<int>> childToParent;
    std::vector<std::set<int>> exclusions;
    int capacity;
    int height;

    explicit HexGraphInternal(int capacity);

    HexGraphInternal(const HexGraphInternal &) = delete;

    void resize(int maxNodes);

    int size() const;

    void add_child(int parent, int child);

    void add_exclusion(int node1, int node2);

    // Identify disconnected components, add exclusion edges between them
    std::map<int, std::set<int>> add_parent_exclusions();

    bool validate();
};

template<class NodeType>
class HexGraph {
protected:
    HexGraphInternal internal;
    int nodeCount = -1;
    std::map<NodeType, int> nodes = {};
    std::vector<NodeType> nodesReverse = {};

public:
    HexGraph(int initial_capacity);

    void add_node(NodeType node);

    void add_child(NodeType node1, NodeType node2);

    void add_exclusion(NodeType node1, NodeType node2);

    std::map<NodeType, std::set<NodeType>> add_parent_exclustions();

    bool validate();
};

template<class NodeType>
void HexGraph<NodeType>::add_node(NodeType node) {
    if (nodes.find(node) == nodes.end()) {
        nodes[node] = ++nodeCount;
        nodesReverse.push_back(node);
    }
}

template<class NodeType>
void HexGraph<NodeType>::add_child(NodeType node1, NodeType node2) {
    add_node(node1);
    add_node(node2);
    auto v = nodes[node1];
    auto v2 = nodes[node2];
    internal.add_child(nodes[node1], nodes[node2]);
}

template<class NodeType>
HexGraph<NodeType>::HexGraph(int initial_capacity): internal(initial_capacity) {

}

template<class NodeType>
bool HexGraph<NodeType>::validate() {
    return internal.validate();
}

template<class NodeType>
std::map<NodeType, std::set<NodeType>> HexGraph<NodeType>::add_parent_exclustions() {
    std::map<NodeType, std::set<NodeType>> exclusions;
    auto parent_exclusions = internal.add_parent_exclusions();
    for (auto ex:parent_exclusions) {
        std::set<NodeType> excluded;
        for (auto exNo:ex.second) {
            excluded.insert(nodesReverse[exNo]);
        }
        exclusions[nodesReverse[ex.first]] = excluded;
    }
    return exclusions;
}

template<class NodeType>
void HexGraph<NodeType>::add_exclusion(NodeType node1, NodeType node2) {
    add_node(node1);
    add_node(node2);
    auto v = nodes[node1];
    auto v2 = nodes[node2];
    internal.add_exclusion(v, v2);
}


#endif //KGER_HEX_H
