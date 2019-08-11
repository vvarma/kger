#include <iostream>
#include <memory>
#include "relation_parser.h"
#include "graph.h"
#include "random_walker.h"
#include <cxxopts.hpp>

using namespace std;

void print_vec(vector<string_view> finds) {
    if (finds.empty()) {
        cout << "empty\n";
    } else {
        cout << finds.size() << '\n';
    }
}

int main(int argc, char *argv[]) {
    cxxopts::Options options("KGer", "Building KGs from TTLs");
    options.add_options()
            ("ttl_list", "ttl file to parse and add", cxxopts::value<vector<string>>())
            ("spm_model", "sentencepiece model to tokenize", cxxopts::value<string>());
    options.add_options("debug")("timeit", "capture time info of each parse run", cxxopts::value<bool>());
    auto results = options.parse(argc, argv);

    relation_parser rp;
    auto vocabulary = make_shared<Vocabulary>(results["spm_model"].as<string>());
    if (!vocabulary->ok) {
        cout << "Could not load vocabulary";
        return 1;
    }
    auto registery = make_shared<Registery>(vocabulary);
    auto builder = make_shared<GraphBuilder>(registery);
    auto lambda = [builder](vector<string_view> finds) -> void {
        builder->relation_collector(finds);
    };

    for (const auto &filename:results["ttl_list"].as<vector<string>>()) {
        cout << "Processing file:" << filename << endl;
        rp.parse(filename, lambda);
    }
    cout << *builder << endl;
    auto entity_edge = registery->get_edge("http://www.w3.org/1999/02/22-rdf-syntax-ns#type")->unique_id();
    auto comment_edge = registery->get_edge("http://www.w3.org/2000/01/rdf-schema#comment")->unique_id();

    registery->prune([entity_edge, comment_edge](shared_ptr<Node> n) -> bool {
        if (n->relations.find(entity_edge) == n->relations.end()) {
            return true;
        }
        return n->relations.find(comment_edge) == n->relations.end();
    });
    map<has_unique_id::uid, int> report;
    report[entity_edge] = 0;
    report[comment_edge] = 0;
    registery->iterate_nodes([entity_edge, comment_edge, &report](shared_ptr<Node> n) -> void {
        if (n->relations.find(entity_edge) != n->relations.end()) {
            ++report[entity_edge];
            for (auto en:n->relations[entity_edge]) {
                auto it = report.find(en);
                if (it != report.end())
                    it->second++;
                else
                    report[en] = 1;
            }
        }
        if (n->relations.find(comment_edge) != n->relations.end()) {
            ++report[comment_edge];
        }
    });
    for (auto r:report) {
        if (r.first == entity_edge || r.first == comment_edge) {
            cout << registery->get_edge(r.first)->name() << ":" << r.second << endl;
        } else {
            cout << registery->get_node(r.first)->get_content() << ":" << r.second << endl;
        }
    }
//    for (const auto &filename:results["text_list"].as<vector<string>>()) {
//        cout << "Processing file:" << filename << endl;
//        rp.parse(filename, lambda);
//    }
    cout << *builder << endl;
//    auto token_edge = registery->get_edge("token")->unique_id();
//    RandomWalker rw(registery, vocabulary, 0.5, token_edge, entity_edge);
//    for (int i = 0; i < 100; ++i) {
//        rw.walk(10);
//        for (auto t:rw.debug) {
//            cout << t << '\t';
//        }
//        cout << endl;
//
//    }
    return 0;
}