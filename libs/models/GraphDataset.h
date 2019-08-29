#include <utility>

//
// Created by nvr on 3/8/19.
//

#ifndef KGER_GRAPHDATASET_H
#define KGER_GRAPHDATASET_H

#include <torch/torch.h>
#include "models/model.h"

struct SequenceExample {
    torch::Tensor data;
    torch::Tensor sizes;
    torch::Tensor labels;

    SequenceExample(torch::Tensor data, torch::Tensor sizes, torch::Tensor labels) : data(std::move(data)),
                                                                                     sizes(std::move(sizes)),
                                                                                     labels(std::move(labels)) {}
};

struct Stack : public torch::data::transforms::Collation<SequenceExample> {
    SequenceExample apply_batch(std::vector<SequenceExample> input_batch) override;
};

struct GraphDataset : public torch::data::datasets::Dataset<GraphDataset, SequenceExample> {
    torch::Device device;
    std::shared_ptr<SequenceLabelDataset> datasetInternal;
    std::vector<size_t> idx;

    GraphDataset(std::shared_ptr<SequenceLabelDataset>, torch::Device device);

    GraphDataset(std::shared_ptr<SequenceLabelDataset>, torch::Device device, std::vector<size_t> idx);

    std::vector<GraphDataset> split(std::vector<float> splits);

    SequenceExample get(size_t index) override;

    c10::optional<size_t> size() const override;

    int get_label_count() const;

    torch::Tensor labelWeights() const;
};

#endif //KGER_GRAPHDATASET_H
