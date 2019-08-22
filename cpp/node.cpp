#include <utility>

//
// Created by nvr on 29/7/19.
//

#include "node.h"

Edge::Edge(std::string resource) : resource(move(resource)) {}

void Node::add_relation(const std::shared_ptr<const Edge> &, const std::shared_ptr<const Node> &) {}

Node::~Node() = default;

std::string NamedEntityNode::get_content() const {
    return label;
}

NamedEntityNode::NamedEntityNode(std::string resource) : EntityNode(std::move(resource)) {

}

void NamedEntityNode::accept(NodeVisitor *visitor) {
    visitor->visit(shared_from_this());

}

EntityNode::EntityNode(std::string resource) : resource(std::move(resource)) {}

EntityNode::~EntityNode() = default;

std::string EntityNode::get_content() const {
    return resource;
}

LexicalNode::LexicalNode(std::vector<int> tokens) : tokens(move(tokens)) {}

std::string LexicalNode::get_content() const {
    return "tokenized";
}

void LexicalNode::accept(NodeVisitor *visitor) {
    visitor->visit(shared_from_this());
}

NodeVisitor::~NodeVisitor() = default;

InstanceNode::InstanceNode(std::string resource) : EntityNode(std::move(resource)) {

}

void InstanceNode::accept(NodeVisitor *visitor) {
    visitor->visit(shared_from_this());
}

void InstanceNode::add_relation(const std::shared_ptr<const Edge> &edge, const std::shared_ptr<const Node> &object) {
    auto it = relations.find(edge->unique_id());
    if (it == relations.end()) {
        relations[edge->unique_id()] = {object->unique_id()};
    } else {
        it->second.insert(object->unique_id());
    }

}

std::ostream &operator<<(std::ostream &os, Graph const &m) {
    return os << "Graph \n" << "Named Entities:" << m.named_entities.size() << '\n'
              << "Instances:" << m.instances.size() << '\n' << "Lexicals:" << m.lexicals.size()
              << '\n' << "Edges:" << m.edges.size() << std::endl;
}

void
Graph::prune(const std::function<bool(std::shared_ptr<InstanceNode>)> &must_remove,
             const std::vector<has_unique_id::uid> &cascading_edges) {
    for (auto it = instances.begin(); it != instances.end();) {
        if (must_remove(it->second)) {
            for (auto e:cascading_edges) {
                auto cascade_it = it->second->relations.find(e);
                if (cascade_it != it->second->relations.end()) {
                    for (auto cascade_node:cascade_it->second) {
                        // dont touch named entities
                        // concurrent modifications?
                        if (instances.find(cascade_node) != instances.end())
                            instances.erase(cascade_node);
                        else if (lexicals.find(cascade_node) != lexicals.end())
                            lexicals.erase(cascade_node);
                    }
                }

            }
            instances.erase(it++);
        } else {
            ++it;
        }
    }
}

void Graph::collect(const std::function<void(std::shared_ptr<InstanceNode>, std::set<std::shared_ptr<NamedEntityNode>>,
                                        std::set<std::shared_ptr<LexicalNode>>)>& collector) {
    for (const auto &p:instances) {
        auto in = p.second;
        std::set<std::shared_ptr<NamedEntityNode>> nens;
        std::set<std::shared_ptr<LexicalNode>> lns;
        for (const auto &r:in->relations) {
            for (auto rInstance:r.second) {
                auto nenIt = named_entities.find(rInstance);
                if (nenIt != named_entities.end()) {
                    nens.insert(nenIt->second);
                    continue;
                }
                auto lnIt = lexicals.find(rInstance);
                if (lnIt != lexicals.end()) {
                    lns.insert(lnIt->second);
                    continue;
                }
            }
        }
        collector(in, nens, lns);
    }
}

