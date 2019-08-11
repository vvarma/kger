//
// Created by nvr on 29/7/19.
//

#ifndef KGER_NODE_H
#define KGER_NODE_H


#include "unique_id.h"
#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <ostream>
#include <functional>


struct NodeVisitor;

struct Edge : has_unique_id {
    std::string resource;

    explicit Edge(std::string resource);
};

struct Node : has_unique_id {
    virtual std::string get_content() const = 0;

    virtual void accept(NodeVisitor *) = 0;

    virtual void add_relation(const std::shared_ptr<const Edge> &, const std::shared_ptr<const Node> &);

    virtual ~Node();
};

struct EntityNode : Node {
    std::string resource;

    explicit EntityNode(std::string resource);

    std::string get_content() const override;

    virtual ~EntityNode();
};

struct NamedEntityNode : EntityNode, std::enable_shared_from_this<NamedEntityNode> {
    std::vector<has_unique_id::uid> parents;
    std::string label;

    explicit NamedEntityNode(std::string resource);

    std::string get_content() const override;

    void accept(NodeVisitor *visitor) override;
};

struct InstanceNode : EntityNode, std::enable_shared_from_this<InstanceNode> {
    std::map<has_unique_id::uid, std::set<has_unique_id::uid >> relations;

    explicit InstanceNode(std::string resource);

    void add_relation(const std::shared_ptr<const Edge> &ptr, const std::shared_ptr<const Node> &sharedPtr) override;

    void accept(NodeVisitor *visitor) override;
};

struct LexicalNode : Node, std::enable_shared_from_this<LexicalNode> {
    std::vector<int> tokens;

    std::string get_content() const override;

    explicit LexicalNode(std::vector<int> tokens);

    void accept(NodeVisitor *visitor) override;
};


struct Graph {
    std::map<has_unique_id::uid, std::shared_ptr<NamedEntityNode>> named_entities;
    std::map<has_unique_id::uid, std::shared_ptr<InstanceNode>> instances;
    std::map<has_unique_id::uid, std::shared_ptr<LexicalNode>> lexicals;
    std::map<has_unique_id::uid, std::shared_ptr<Edge>> edges;

    void prune(const std::function<bool(std::shared_ptr<InstanceNode>)> &must_remove,
               const std::vector<has_unique_id::uid> &cascading_edges);

    void collect(const std::function<void(std::shared_ptr<InstanceNode>, std::set<std::shared_ptr<NamedEntityNode>>,
                                     std::set<std::shared_ptr<LexicalNode>>)>& collector);
};

std::ostream &operator<<(std::ostream &os, Graph const &m);

struct NodeVisitor {
    virtual ~NodeVisitor();

    virtual void visit(std::shared_ptr<NamedEntityNode>) = 0;

    virtual void visit(std::shared_ptr<InstanceNode>) = 0;

    virtual void visit(std::shared_ptr<LexicalNode>) = 0;
};


#endif //KGER_NODE_H
