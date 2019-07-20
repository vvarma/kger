#include <utility>

//
// Created by nvr on 18/7/19.
//

#include "graph.h"


void GraphBuilder::relation_collector(vector<string_view> finds) {
    if (finds.size() != 3) {
        return;
    }
    shared_ptr<Node> subject = parse_entity(finds[0]);
    shared_ptr<Node> object = parse_entity(finds[2]);
    shared_ptr<Edge> predicate = parse_relation(finds[1]);

    subject->add_relation(predicate, object);
    object->add_relation(predicate, subject);
}

shared_ptr<Node> GraphBuilder::parse_entity(string_view view) {
    return registery->get_node(view);
}

shared_ptr<Edge> GraphBuilder::parse_relation(string_view edge) {
    return registery->get_edge(edge);
}

GraphBuilder::GraphBuilder(unique_ptr<Registery> r) : registery(move(r)) {

}
std::ostream &operator<<(std::ostream &os, GraphBuilder const &m) {
    return os << "Graph Builder stats\n"
              << "Found nodes: " << m.registery->node_count() << '\n'
              << "Found edges: " << m.registery->edge_count();
}


shared_ptr<Node> Registery::get_node(string_view node) {
    auto n = string(node);
    if (node.find("http") == 0) {
        auto it = nodes.find(node);
        if (it == nodes.end()) {
            auto entity = make_shared<EntityNode>(n);
            nodes[entity->get_content()] = static_pointer_cast<Node>(entity);
        }
        return nodes[node];
    } else {
        return static_pointer_cast<Node>(make_shared<LexicalNode>(n));
    }
}

shared_ptr<Edge> Registery::get_edge(string_view edge) {
    auto e = string(edge);
    auto it = edges.find(edge);
    if (it == edges.end()) {
        auto _edge= make_shared<Edge>(e);
        edges[_edge->name()] = _edge;
    }
    return edges[e];
}

size_t Registery::node_count() {
    return nodes.size();
}

size_t Registery::edge_count() {
    return edges.size();
}

EntityNode::EntityNode(string node) : Node(std::move(node)) {

}


LexicalNode::LexicalNode(string content) : Node(std::move(content)) {

}

Node::~Node() {

}

Node::Node(string content) : content(std::move(content)) {

}

void Node::add_relation(const shared_ptr<Edge> &edge, shared_ptr<Node> node) {
    auto it = relations.find(edge);
    if (it == relations.end()) {
        relations[edge] = {std::move(node)};
    } else {
        auto nodes = relations[edge];
        for (const shared_ptr<Node> &n:nodes) {
            if (n == node) {
                return;
            }
        }
        nodes.push_back(node);
        relations[edge] = nodes;
    }
}

string_view Node::get_content() {
    return content;
}

Edge::Edge(string entity) : entity(std::move(entity)) {}

string_view Edge::name() {
    return entity;
}
