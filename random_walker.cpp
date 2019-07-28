#include <utility>
#include <iostream>

//
// Created by nvr on 21/7/19.
//

#include "random_walker.h"

default_random_engine RandomWalker::generator;

void RandomWalker::visit(const EntityNode &node) {
    set<has_unique_id::uid> tokenSet;
    set<has_unique_id::uid> entitySet;
    auto token_it = node.relations.find(token_edge);
    if (token_it != node.relations.end() && !token_it->second.empty()) {
        tokenSet = token_it->second;
    }
    auto entity_it = node.relations.find(entity_edge);
    if (entity_it != node.relations.end() && !entity_it->second.empty()) {
        entitySet = entity_it->second;
    }
    bool tokenVisit = false;
    if (entitySet.empty() && tokenSet.empty()) {
        seed = 0;
        return;
    } else if (!entitySet.empty() && !tokenSet.empty()) {
        if (distribution(generator))
            tokenVisit = true;
    } else if (!tokenSet.empty()) {
        tokenVisit = true;
    }
    if (tokenVisit) {
        vector<float> idfs;
        vector<has_unique_id::uid> ids;
        for (auto id:token_it->second) {
            auto tn = dynamic_pointer_cast<TokenNode>(kg->get_node(id));
            auto docCount = vocabulary->get_doc_count(tn->getVocabId());
            if (docCount > 0) {
                idfs.push_back(
                        (float) tn->get_term_count(node.unique_id()) /
                        (float) vocabulary->get_doc_count(tn->getVocabId()));
                ids.push_back(id);
            }
        }
        discrete_distribution d(idfs.begin(), idfs.end());
        seed = ids[d(generator)];
        return;
    }

    auto it = entity_it->second.begin();
    advance(it, generator() % entity_it->second.size());
    seed = *it;
}

void RandomWalker::visit(const LexicalNode &node) {
    seed = 0;
}

void RandomWalker::visit(const TokenNode &node) {
    auto it = node.relations.find(token_edge);
    if (it == node.relations.end()) {
        seed = 0;
        return;
    }
    if (it->second.empty()) {
        seed = 0;
        return;
    }
    vector<float> idfs;
    vector<has_unique_id::uid> ids;
    for (auto id:it->second) {
        auto docCount = vocabulary->get_doc_count(node.getVocabId());
        if (docCount > 0) {
            idfs.push_back(
                    (float) node.get_term_count(id) /
                    (float) vocabulary->get_doc_count(node.getVocabId()));
            ids.push_back(id);
        }
    }
    discrete_distribution d(idfs.begin(), idfs.end());
    seed = ids[d(generator)];
}

RandomWalker::RandomWalker(shared_ptr<Registery> kg, shared_ptr<Vocabulary> vocabulary, float lambda,
                           has_unique_id::uid token_edge, has_unique_id::uid entity_edge) : kg(std::move(kg)),
                                                                                            vocabulary(std::move(
                                                                                                    vocabulary)),
                                                                                            distribution(lambda),
                                                                                            token_edge(token_edge),
                                                                                            entity_edge(entity_edge) {


}

void
RandomWalker::walk(int max_depth, int curr_depth) {
    if (curr_depth >= max_depth) {
        seed = 0;
        return;
    }
    if (curr_depth > 0 && seed == 0) {
        return;
    }
    if (seed == 0) {
        auto it = kg->nodes.begin();
        std::advance(it, generator() % kg->nodes.size());
        seed = it->first;
        trace.clear();
        debug.clear();
    }
    trace.push_back(seed);
    debug.push_back(kg->nodes[seed]->get_content());
    kg->nodes[seed]->accept(this);
    walk(max_depth, ++curr_depth);
}
