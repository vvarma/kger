//
// Created by nvr on 21/7/19.
//

#ifndef KGER_RANDOM_WALKER_H
#define KGER_RANDOM_WALKER_H

#include "graph.h"
#include <random>

using namespace std;

class RandomWalker : NodeVisitor {
    static default_random_engine generator;

    void visit(const EntityNode &node) override;

    void visit(const LexicalNode &node) override;

    void visit(const TokenNode &node) override;

    shared_ptr<Registery> kg;
    shared_ptr<Vocabulary> vocabulary;
    bernoulli_distribution distribution;

    has_unique_id::uid seed = 0;

    has_unique_id::uid token_edge, entity_edge;

public:
    vector<has_unique_id::uid> trace;
    vector<string_view> debug;

    RandomWalker(shared_ptr<Registery> kg, shared_ptr<Vocabulary> vocabulary, float lambda,
                 has_unique_id::uid token_edge, has_unique_id::uid entity_edge);

    void walk(int max_depth, int curr_depth = 0);


};

#endif //KGER_RANDOM_WALKER_H
