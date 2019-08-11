//
// Created by nvr on 30/7/19.
//

#include "nodebuilder.h"
#include "owlparser.h"

void GraphBuilder::parse_ontologies(const std::string &filename) {
    OntologyBuilder owl;
    owl.processTree(filename);
    for (const auto &p:owl.nodes) {
        p.second->accept(this);
    }
}

GraphBuilder::GraphBuilder(std::shared_ptr<Vocabulary> vocabulary) : graph(std::make_shared<Graph>()),
                                                                vocabulary(move(vocabulary)) {
}

GraphBuilder::GraphBuilder(std::shared_ptr<Graph> graph, std::shared_ptr<Vocabulary> vocabulary) : graph(std::move(graph)),
                                                                                         vocabulary(move(vocabulary)) {

}

std::shared_ptr<Node> parse_entity(std::string_view node, const std::map<std::string_view, std::shared_ptr<EntityNode>> &node_cache,
                              const std::shared_ptr<Vocabulary> &vocabulary);

std::shared_ptr<Edge> parse_relation(std::string_view edge, const std::map<std::string_view, std::shared_ptr<Edge>> &edge_cache);

void GraphBuilder::relation_collector(const std::vector<std::string_view> &finds) {
    if (finds.size() != 3) {
        return;
    }
    std::shared_ptr<Node> subject = parse_entity(finds[0], entities, vocabulary);
    std::shared_ptr<Node> object = parse_entity(finds[2], entities, vocabulary);
    std::shared_ptr<Edge> predicate = parse_relation(finds[1], edge_cache);
//    auto translatedPredicate = predicate->translation();
    if (subject && object && predicate) {
        subject->accept(this);
        object->accept(this);
        if (graph->edges.find(predicate->unique_id()) == graph->edges.end()) {
            graph->edges[predicate->unique_id()] = predicate;
            edge_cache[predicate->resource] = predicate;
        }
        subject->add_relation(predicate, object);
    }
}

void GraphBuilder::visit(std::shared_ptr<NamedEntityNode> ptr) {
    if (graph->named_entities.find(ptr->unique_id()) == graph->named_entities.end()) {
        graph->named_entities[ptr->unique_id()] = ptr;
        entities[ptr->resource] = ptr;
    }
}

void GraphBuilder::visit(std::shared_ptr<InstanceNode> ptr) {
    if (graph->instances.find(ptr->unique_id()) == graph->instances.end()) {
        graph->instances[ptr->unique_id()] = ptr;
        entities[ptr->resource] = ptr;
    }
}

void GraphBuilder::visit(std::shared_ptr<LexicalNode> ptr) {
    graph->lexicals[ptr->unique_id()] = ptr;
}


std::shared_ptr<Node> parse_entity(std::string_view node, const std::map<std::string_view, std::shared_ptr<EntityNode>> &node_cache,
                              const std::shared_ptr<Vocabulary> &vocabulary) {
    if (node.find("http") == 0) {
        auto it = node_cache.find(node);
        if (it != node_cache.end()) {
            return it->second;
        }
        return std::make_shared<InstanceNode>(std::string(node));
    } else {
        auto tokens = vocabulary->tokenize(std::string(node));
        return std::make_shared<LexicalNode>(tokens);
    }
}

std::shared_ptr<Edge> parse_relation(std::string_view edge, const std::map<std::string_view, std::shared_ptr<Edge>> &edge_cache) {
    auto it = edge_cache.find(edge);
    if (it != edge_cache.end()) {
        return it->second;
    }
    auto e = std::string(edge);
    return std::make_shared<Edge>(e);
}
