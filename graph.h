//
// Created by nvr on 18/7/19.
//

#ifndef KGER_GRAPH_H
#define KGER_GRAPH_H

#include <vector>
#include <map>
#include <memory>
#include <ostream>

using namespace std;

class Edge {
public:
    Edge(string entity);

    string_view name();

private:
    const string entity;
};


class Node {
    const string content;
    map<shared_ptr<Edge>, vector<shared_ptr<Node>>> relations;
public:
    explicit Node(string content);

    void add_relation(const shared_ptr<Edge> &edge, shared_ptr<Node> node);

    string_view get_content();

    virtual ~Node();

};

class LexicalNode : public Node {
public:
    explicit LexicalNode(string content);
};

class EntityNode : public Node {
public:
    explicit EntityNode(string content);
};

class Registery {
    map<string_view, shared_ptr<Node>> nodes;
    map<string_view, shared_ptr<Edge>> edges;
public:
    shared_ptr<Node> get_node(string_view node);

    shared_ptr<Edge> get_edge(string_view edge);

    size_t node_count();

    size_t edge_count();
};

class GraphBuilder {
    unique_ptr<Registery> registery;

    friend std::ostream &operator<<(std::ostream &os, GraphBuilder const &m);

public:
    explicit GraphBuilder(unique_ptr<Registery> r);

    void relation_collector(vector<string_view> finds);


private:
    shared_ptr<Node> parse_entity(string_view view);

    shared_ptr<Edge> parse_relation(string_view edge);
};

#endif //KGER_GRAPH_H
