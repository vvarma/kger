#include <utility>
#include <iostream>

//
// Created by nvr on 18/7/19.
//

#include "graph.h"
#include "unique_id.h"


void GraphBuilder::relation_collector(vector<string_view> finds) {
    if (finds.size() != 3) {
        return;
    }
    shared_ptr<Node> subject = parse_entity(finds[0]);
    shared_ptr<Node> object = parse_entity(finds[2]);
    shared_ptr<Edge> predicate = parse_relation(finds[1]);
    auto translatedPredicate = predicate->translation();
    if (translatedPredicate.empty()) {
        subject->add_relation(predicate, object);
        object->add_relation(predicate, subject);
    } else if (translatedPredicate == "token") {
//        if (subject->relations.empty())
//            return;
        predicate = registery->get_edge(translatedPredicate);
        auto termCounts = registery->get_tokenised_nodes(object->get_content());
        for (const auto &p:termCounts) {
            p.first->add_term_count(subject->unique_id(), p.second);
            subject->add_relation(predicate, p.first);
            p.first->add_relation(predicate, subject);
        }
    }
}

shared_ptr<Node> GraphBuilder::parse_entity(string_view view) {
    return registery->get_node(view);
}

shared_ptr<Edge> GraphBuilder::parse_relation(string_view edge) {
    return registery->get_edge(edge);
}

GraphBuilder::GraphBuilder(shared_ptr<Registery> r) : registery(move(r)) {
}

std::ostream &operator<<(std::ostream &os, GraphBuilder const &m) {
    return os << "Graph Builder stats\n"
              << "Found nodes: " << m.registery->node_count() << '\n'
              << "Found edges: " << m.registery->edge_count();
}


shared_ptr<Node> Registery::get_node(string_view node) {
    if (node.find("http") == 0) {
        if (node.find_last_of('/') != string_view::npos) {
            node = node.substr(node.find_last_of('/'), string_view::npos);
        }
        auto it = node_cache.find(node);
        if (it == node_cache.end()) {
            auto n = string(node);
            auto entity = make_shared<EntityNode>(n);
            node_cache[entity->get_content()] = entity->unique_id();
            nodes[entity->unique_id()] = static_pointer_cast<Node>(entity);
        }
        return nodes[node_cache[node]];
    } else {
        auto n = string(node);
        auto entity = static_pointer_cast<Node>(make_shared<LexicalNode>(n));
        //nodes[entity->unique_id()] = entity;
        return entity;
    }
}

shared_ptr<Edge> Registery::get_edge(string_view edge) {
    auto e = string(edge);
    auto it = edge_cache.find(edge);
    if (it == edge_cache.end()) {
        auto _edge = make_shared<Edge>(e);
        edge_cache[_edge->name()] = _edge->unique_id();
        edges[_edge->unique_id()] = _edge;
    }
    return edges[edge_cache[edge]];
}

size_t Registery::node_count() {
    return nodes.size();
}

size_t Registery::edge_count() {
    return edges.size();
}

shared_ptr<Node> Registery::get_node(has_unique_id::uid id) {
    return nodes[id];
}

shared_ptr<Edge> Registery::get_edge(has_unique_id::uid id) {
    return edges[id];
}

Registery::Registery(shared_ptr<Vocabulary> v) : vocabulary(move(v)) {

}

map<shared_ptr<TokenNode>, int> Registery::get_tokenised_nodes(string_view content) {
    auto termFrequencies = vocabulary->process(string(content));
    map<shared_ptr<TokenNode>, int> termNodes;
    for (pair<int, int> tf:termFrequencies) {
        auto it = token_node_cache.find(tf.first);
        if (it == token_node_cache.end()) {
            auto tn = make_shared<TokenNode>(vocabulary->get_token(tf.first), tf.first);
            token_node_cache[tf.first] = tn->unique_id();
            nodes[tn->unique_id()] = tn;
            termNodes[tn] = tf.second;
        } else {
            auto tn = dynamic_pointer_cast<TokenNode>(nodes[token_node_cache[tf.first]]);
            if (tn->is_stop) {
                continue;
            } else if (vocabulary->is_stop(tn->getVocabId(), 0.1)) {
                tn->mark_as_stop();
            } else {
                termNodes[tn] = tf.second;
            }

        }
    }
    return termNodes;

}

void Registery::print_size() const {
    cout << "size of registery:" << sizeof(this) << endl;

}

EntityNode::EntityNode(string node) : ContentNode(std::move(node)) {

}

void EntityNode::accept(NodeVisitor *visitor) const {
    visitor->visit(*this);

}


LexicalNode::LexicalNode(string content) : ContentNode(std::move(content)) {

}

void LexicalNode::accept(NodeVisitor *visitor) const {
    visitor->visit(*this);
}

Node::~Node() {

}

ContentNode::ContentNode(string content) : content(std::move(content)) {

}

string_view ContentNode::get_content() const {
    return content;
}

Node::Node() {};

void Node::add_relation(const shared_ptr<Edge> &edge, const shared_ptr<Node> &node) {
    auto it = relations.find(edge->unique_id());
    if (it == relations.end()) {
        relations[edge->unique_id()] = {node->unique_id()};
    } else {
        relations[edge->unique_id()].insert(node->unique_id());
    }
}

string_view Node::get_content() const {
    return "empty";
}


Edge::Edge(string entity) : entity(std::move(entity)) {}

string_view Edge::name() {
    return entity;
}

string Edge::translation() {
    if (entity.find("comment") != string::npos)
        return "token";
    return "";
}

TokenNode::TokenNode(string token, int vocabId) : ContentNode(token), vocabId(vocabId) {

}

void TokenNode::add_term_count(has_unique_id::uid id, int count) {
    termCounts[id] = count;

}

void TokenNode::accept(NodeVisitor *visitor) const {
    visitor->visit(*this);

}

int TokenNode::get_term_count(has_unique_id::uid id) const {
    auto it = termCounts.find(id);
    if (it == termCounts.end()) {
        return 0;
    }
    return it->second;
}

int TokenNode::getVocabId() const {
    return vocabId;
}

void TokenNode::mark_as_stop() {
    is_stop = true;
    relations.clear();
    cout << "marked token as stop:" << get_content() << endl;
}
