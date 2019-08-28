#include <utility>

//
// Created by nvr on 28/8/19.
//
#include <memory>
#include "models/trainer.h"
#include <torch/torch.h>
#include <GraphDataset.h>
#include "EntityModeller.h"

void train(std::shared_ptr<SequenceLabelDataset> sequence_label_dataset, const ModelOptions &options) {
    torch::Device device = torch::kCPU;
    if (torch::cuda::is_available()) {
        std::cout << "CUDA is available! Training on GPU." << std::endl;
        device = torch::kCUDA;
    }
    auto graph_dataset = GraphDataset(std::move(sequence_label_dataset), device);
    auto label_weights = graph_dataset.labelWeights();
    std::cout << "weight sum:" << label_weights.sum() << std::endl;
    auto dataset = graph_dataset.map(Stack());
    auto batches_per_epoch = dataset.size();
    auto loader = torch::data::make_data_loader(std::move(dataset),
                                                torch::data::DataLoaderOptions().batch_size(1024).workers(3));
    EntityModeller builder(options.max_num_tokens, graph_dataset.get_label_count());
    builder->to(device);

    auto optimizer = torch::optim::Adam(builder->parameters(), torch::optim::AdamOptions(3e-3));
    auto kNumberOfEpochs = 50;
    for (int64_t epoch = 1; epoch <= kNumberOfEpochs; ++epoch) {
        int64_t batch_index = 0;
        for (SequenceExample &batch : *loader) {
            builder->zero_grad();
            auto op = builder(batch.data, batch.sizes);
            auto loss = torch::nll_loss(op, batch.labels, label_weights
            );
            loss.backward();
            optimizer.step();
            if (++batch_index % 1000 == 0) {
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

}
