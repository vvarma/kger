//
// Created by nvr on 18/7/19.
//

#ifndef KGER_GRAPH_H
#define KGER_GRAPH_H

#include <vector>
#include <map>
#include <memory>
#include <ostream>
#include <atomic>
#include <set>
#include <functional>
#include "unique_id.h"
#include "vocabulary.h"

using namespace std;

class Edge : public has_unique_id {
public:
    Edge(string entity);

    string_view name();

    string translation();

private:
    const string entity;
};

class NodeVisitor;

struct Node : public has_unique_id {
    map<uid, set<uid>> relations;

    explicit Node();

    void add_relation(const shared_ptr<Edge> &edge, const shared_ptr<Node> &node);

    virtual string_view get_content() const;

    virtual void accept(NodeVisitor *visitor) const = 0;

    virtual ~Node();
};

class ContentNode : public Node {
    const string content;
public:
    explicit ContentNode(string content);

    string_view get_content() const override;

};

class LexicalNode : public ContentNode {
public:
    explicit LexicalNode(string content);

    void accept(NodeVisitor *visitor) const override;
};

class EntityNode : public ContentNode {
public:
    explicit EntityNode(string content);

    void accept(NodeVisitor *visitor) const override;
};

class TokenNode : public ContentNode {
    int vocabId;
    map<has_unique_id::uid, int> termCounts;
public:
    bool is_stop;

    explicit TokenNode(string token, int vocabId);

    void add_term_count(has_unique_id::uid id, int count);

    int get_term_count(has_unique_id::uid) const;

    int getVocabId() const;

    void accept(NodeVisitor *visitor) const override;

    void mark_as_stop();
};

class NodeVisitor {
public:
    virtual void visit(const EntityNode &) = 0;

    virtual void visit(const LexicalNode &) = 0;

    virtual void visit(const TokenNode &) = 0;
};

class RandomWalker;

class Registery {
    shared_ptr<Vocabulary> vocabulary;
    map<string_view, has_unique_id::uid> node_cache;
    map<string_view, has_unique_id::uid> edge_cache;
    map<int, has_unique_id::uid> token_node_cache;
    map<has_unique_id::uid, shared_ptr<Node>> nodes;
    map<has_unique_id::uid, shared_ptr<Edge>> edges;
public:
    Registery(shared_ptr<Vocabulary> v);

    shared_ptr<Node> get_node(string_view node);

    shared_ptr<Edge> get_edge(string_view edge);

    shared_ptr<Node> get_node(has_unique_id::uid id);

    shared_ptr<Edge> get_edge(has_unique_id::uid id);

    size_t node_count();

    size_t edge_count();

    void prune(const function<bool(shared_ptr<Node>)> & must_remove);

    void iterate_nodes(const function<void(shared_ptr<Node>)>&reporter);

    void print_size() const;

    friend RandomWalker;
};

class GraphBuilder {
    shared_ptr<Registery> registery;

    friend std::ostream &operator<<(std::ostream &os, GraphBuilder const &m);

public:
    explicit GraphBuilder(shared_ptr<Registery> r);

    void relation_collector(vector<string_view> finds);


private:
    shared_ptr<Node> parse_entity(string_view view);

    shared_ptr<Edge> parse_relation(string_view edge);
};

#endif //KGER_GRAPH_H
