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
            ("text_list", "ttl file of abstract to support to parse and add", cxxopts::value<vector<string>>())
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
    for (const auto &filename:results["text_list"].as<vector<string>>()) {
        cout << "Processing file:" << filename << endl;
        rp.parse(filename, lambda);
    }
    cout << *builder << endl;
    auto token_edge = registery->get_edge("token")->unique_id();
    auto entity_edge = registery->get_edge("http://www.w3.org/1999/02/22-rdf-syntax-ns#type")->unique_id();
    RandomWalker rw(registery, vocabulary, 0.5, token_edge, entity_edge);
    for (int i = 0; i < 100; ++i) {
        rw.walk(10);
        for (auto t:rw.debug) {
            cout << t << '\t';
        }
        cout << endl;

    }
    return 0;
}