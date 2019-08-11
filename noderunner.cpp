#include <utility>

#include "nodebuilder.h"
#include "relation_parser.h"
#include "vocabulary.h"
#include "GraphDataset.h"
#include "EntityModeller.h"
#include <iostream>
#include <cxxopts.hpp>

//
// Created by nvr on 30/7/19.
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

    relation_parser rp;
    auto lambda = [&b](std::vector<std::string_view> finds) -> void {
        b.relation_collector(finds);
    };
    for (const auto &filename:ttl_list) {
        rp.parse(filename, lambda);
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
            ("ttl_list", "ttl file to parse and add", cxxopts::value<std::vector<std::string>>())
            ("spm_model", "sentencepiece model to tokenize", cxxopts::value<std::string>());
    options.add_options("debug")("timeit", "capture time info of each parse run", cxxopts::value<bool>());
    auto results = options.parse(argc, argv);
    auto vocabulary = get_vocabulary(results["spm_model"].as<std::string>());
    auto graph = build_graph(vocabulary, results["ttl_list"].as<std::vector<std::string>>());
    torch::Device device = torch::kCPU;
    if (torch::cuda::is_available()) {
        std::cout << "CUDA is available! Training on GPU." << std::endl;
        device = torch::kCUDA;
    }
    auto graph_dataset = GraphDataset(std::move(graph), vocabulary, device);
    auto dataset = graph_dataset.map(Stack());
    auto batches_per_epoch = dataset.size();
    auto loader = torch::data::make_data_loader(std::move(dataset),
                                                torch::data::DataLoaderOptions().batch_size(1024).workers(3));
    EntityModeller builder(vocabulary, graph_dataset.get_label_count());
    builder->to(device);

    auto optimizer = torch::optim::Adam(builder->parameters(), torch::optim::AdamOptions(3e-3));
    auto kNumberOfEpochs = 50;
    for (int64_t epoch = 1; epoch <= kNumberOfEpochs; ++epoch) {
        int64_t batch_index = 0;
        for (SequenceExample &batch : *loader) {
            builder->zero_grad();
            auto op = builder(batch.data, batch.sizes);
            auto loss = torch::nll_loss(op, batch.labels);
            loss.backward();
            optimizer.step();
            if (++batch_index%1000==0){
                std::printf(
                        "\n[%2ld/%2d][%3ld/%3ld] loss: %.4f",
                        epoch,
                        kNumberOfEpochs,
                        batch_index,
                        batches_per_epoch.value() / 1024,
                        loss.item<float>());
            }
        }
        int num_cases = 0;
        float correct_cases = 0;

        for (SequenceExample &batch: *loader) {
            auto op = builder(batch.data, batch.sizes);
            auto predicted_class = torch::argmax(op, 1);
            auto targets = batch.labels;
            torch::Tensor sum = targets.eq(predicted_class).sum().cpu();
            correct_cases += sum.item<int>();
            num_cases += targets.sizes()[0];
        }
        std::printf(
                "\n[%2ld/%2d][%3ld/%3ld] Training accuracy: %.4f",
                epoch,
                kNumberOfEpochs,
                ++batch_index,
                batches_per_epoch.value(),
                correct_cases / num_cases);
    }
//    for (int i = 0; i < 10; ++i) {
//        auto e = dataset.get(i);
//        std::cout << "example " << i << std::endl;
//        std::cout << e.data << std::endl;
//        std::cout << e.target << std::endl;
//    }
}

