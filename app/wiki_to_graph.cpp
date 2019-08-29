#include <cxxopts.hpp>
#include <vocabulary/vocabulary.h>
#include <graph/nodebuilder.h>
#include <wikiparser/relation_parser.h>
#include <graph/graph_io.h>

//
// Created by nvr on 24/8/19.
//
std::shared_ptr<Vocabulary> get_vocabulary(std::string model_path) {
    auto vocabulary = std::make_shared<Vocabulary>(model_path);
    if (!vocabulary->ok) {
        std::cout << "Could not load vocabulary";
        // throw exception?
    }
    return vocabulary;
}

std::shared_ptr<Graph> build_graph(std::shared_ptr<Vocabulary> vocabulary, const std::vector<std::string> &ttl_list) {
    GraphBuilder b(std::move(vocabulary));
    b.parse_ontologies("/home/nvr/CLionProjects/kger/data/ontology.owl");

    auto lambda = [&b](std::vector<std::string_view> finds) -> void {
        b.relation_collector(finds);
    };
    for (const auto &filename:ttl_list) {
        nvr::parserelation(filename, lambda);
    }
    std::cout << *b.graph << std::endl;
    std::cout << "Pruning graph" << std::endl;
    std::shared_ptr<Edge> type_edge;
    std::shared_ptr<Edge> lexical_edge;
    std::vector<has_unique_id::uid> cascading_edges;
    auto it = b.edge_cache.find("http://www.w3.org/2000/01/rdf-schema#comment");
    if (it != b.edge_cache.end()) {
        lexical_edge = it->second;
        cascading_edges.push_back(it->second->unique_id());
    }
    it = b.edge_cache.find("http://www.w3.org/1999/02/22-rdf-syntax-ns#type");
    if (it != b.edge_cache.end()) {
        type_edge = it->second;
    }
    if (lexical_edge && type_edge) {
        b.graph->prune([&lexical_edge, &type_edge](std::shared_ptr<InstanceNode> node) -> bool {
            return (node->relations.find(lexical_edge->unique_id()) == node->relations.end() ||
                    node->relations.find(type_edge->unique_id()) == node->relations.end());
        }, cascading_edges);
    }
    std::cout << *b.graph << std::endl;
    return b.graph;
}

int main(int argc, char *argv[]) {
    cxxopts::Options options("KGer2", "Building KGs from TTLs");
    options.add_options()
            ("ttl_list", "ttl file to parse and add", cxxopts::value<std::vector<std::string >>())
            ("spm_model", "sentencepiece model to tokenize", cxxopts::value<std::string>());
    options.add_options("debug")("timeit", "capture time info of each parse run", cxxopts::value<bool>());
    auto results = options.parse(argc, argv);
    auto vocabulary = get_vocabulary(results["spm_model"].as<std::string>());
    auto graph = build_graph(vocabulary, results["ttl_list"].as<std::vector<std::string >>());
    GraphSaveOptions gso;
    gso.output_path = "/tmp/kger";
    save(graph, gso);
    std::cout << "saved graph. loading and testing" << std::endl;
    auto loadedGraph = load(gso);
    std::cout << *loadedGraph << std::endl;
}

