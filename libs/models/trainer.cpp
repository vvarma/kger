#include <utility>

#include <utility>

//
// Created by nvr on 28/8/19.
//
#include <memory>
#include "models/trainer.h"
#include <torch/torch.h>
#include <GraphDataset.h>
#include "EntityModeller.h"

std::tuple<GraphDataset, GraphDataset, GraphDataset> split_dataset(GraphDataset &dataset, std::vector<float> splits) {
    auto split_dataset = dataset.split(std::move(splits));
    return std::make_tuple(split_dataset[0], split_dataset[1], split_dataset[2]);
}

void print_accuracy(std::string mode, torch::data::Iterator<SequenceExample> begin,
                    const torch::data::Iterator<SequenceExample> &end,
                    EntityModeller &model, int epoch, int max_epoch, int batch_num) {

    int num_cases = 0;
    float correct_cases = 0;
    while (begin != end) {
        auto batch = *begin;
        auto op = model(batch.data, batch.sizes);
        auto predicted_class = torch::argmax(op, 1);
        auto targets = batch.labels;
        torch::Tensor sum = targets.eq(predicted_class).sum().cpu();
        correct_cases += sum.item<int>();
        num_cases += targets.sizes()[0];
        ++begin;
    }

    std::printf(
            "\n[%2d/%2d][%3d] %s accuracy: %.4f",
            epoch,
            max_epoch,
            batch_num,
            mode.c_str(),
            correct_cases / num_cases);

}

void train(std::shared_ptr<SequenceLabelDataset> sequence_label_dataset, const ModelOptions &options) {
    torch::Device device = torch::kCPU;
    if (torch::cuda::is_available()) {
        std::cout << "CUDA is available! Training on GPU." << std::endl;
        device = torch::kCUDA;
    }
    auto graph_dataset = GraphDataset(std::move(sequence_label_dataset), device);
    auto[train, validate, test] = split_dataset(graph_dataset, options.splits);

    auto label_weights = train.labelWeights();

    auto train_dataset = train.map(Stack());
    auto train_loader = torch::data::make_data_loader(std::move(train_dataset),
                                                      torch::data::DataLoaderOptions().batch_size(
                                                              options.batch_size).workers(
                                                              options.num_dataloader_workers));
    auto test_dataset = train.map(Stack());
    auto test_loader = torch::data::make_data_loader(std::move(test_dataset),
                                                     torch::data::DataLoaderOptions().batch_size(
                                                             options.batch_size).workers(
                                                             options.num_dataloader_workers));


    EntityModeller builder(options.max_num_tokens, train.get_label_count());
    builder->to(device);
    auto optimizer = torch::optim::Adam(builder->parameters(), torch::optim::AdamOptions(3e-3));

    for (int64_t epoch = 1; epoch <= options.num_epochs; ++epoch) {
        int64_t batch_index = 0;
        for (SequenceExample &batch : *train_loader) {
            builder->zero_grad();
            auto op = builder(batch.data, batch.sizes);
            auto loss = torch::nll_loss(op, batch.labels, label_weights);
            loss.backward();
            optimizer.step();
            ++batch_index;
        }
        print_accuracy("Training", train_loader->begin(), train_loader->end(), builder, epoch, options.num_epochs,
                       batch_index);
        print_accuracy("Testing", test_loader->begin(), test_loader->end(), builder, epoch, options.num_epochs,
                       batch_index);

    }
}
