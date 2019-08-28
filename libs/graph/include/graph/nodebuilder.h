//
// Created by nvr on 30/7/19.
//

#ifndef KGER_NODEBUILDER_H
#define KGER_NODEBUILDER_H

//#include "vocabulary/vocabulary.h"
#include "graph/node.h"


class Vocabulary;

struct GraphBuilder : NodeVisitor {
    std::shared_ptr<Graph> graph;
    std::shared_ptr<Vocabulary> vocabulary;

    std::map<std::string_view, std::shared_ptr<EntityNode>> entities;
    std::map<std::string_view, std::shared_ptr<Edge>> edge_cache;

    explicit GraphBuilder(std::shared_ptr<Vocabulary> vocabulary);

    explicit GraphBuilder(std::shared_ptr<Graph> graph, std::shared_ptr<Vocabulary> vocabulary);

    void parse_ontologies(const std::string &filename);

    void relation_collector(const std::vector<std::string_view> &finds);

    void visit(std::shared_ptr<NamedEntityNode> ptr) override;

    void visit(std::shared_ptr<InstanceNode> ptr) override;

    void visit(std::shared_ptr<LexicalNode> ptr) override;
};


#endif //KGER_NODEBUILDER_H
