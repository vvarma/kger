#include <random>

//
// Created by nvr on 3/8/19.
//

#include "GraphDataset.h"

SequenceExample GraphDataset::get(size_t index) {
    auto i = idx[index];
    auto v = datasetInternal->tokens[i];
    // not using padding id since its -1
    v.resize(datasetInternal->max_token_length, 0);
    std::vector<int> nv(v.begin(), v.begin() + 80);
    auto t = torch::tensor(nv, torch::TensorOptions().dtype(torch::kInt64).requires_grad(false).device(device));
    auto l = torch::scalar_tensor(datasetInternal->label_map[datasetInternal->labels[i]],
                                  torch::TensorOptions().dtype(torch::kLong).requires_grad(
                                          false).device(device));
    auto s = torch::scalar_tensor(static_cast<long>(v.size()),
                                  torch::TensorOptions().dtype(torch::kLong).requires_grad(false).device(device));
    return SequenceExample(t, s, l);
}


c10::optional<size_t> GraphDataset::size() const {
    return idx.size();
}

int GraphDataset::get_label_count() const {
    return datasetInternal->label_count;
}

GraphDataset::GraphDataset(std::shared_ptr<SequenceLabelDataset> dataset, torch::Device device) : datasetInternal(
        std::move(
                dataset)), device(device) {
    idx.resize(datasetInternal->tokens.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::shuffle(idx.begin(), idx.end(), std::mt19937(std::random_device()()));
}

GraphDataset::GraphDataset(std::shared_ptr<SequenceLabelDataset> dataset, torch::Device device, std::vector<size_t> idx)
        : datasetInternal(std::move(dataset)), device(device), idx(std::move(idx)) {}

torch::Tensor GraphDataset::labelWeights() const {
    std::vector<float> w(datasetInternal->label_count);
    for (auto p:datasetInternal->label_count_map) {
        auto it = datasetInternal->label_map.find(p.first);
        w[it->second] = static_cast<float>(p.second) / datasetInternal->labels.size();
    }
    return torch::tensor(w, torch::TensorOptions().device(device).requires_grad(false));
}

std::vector<GraphDataset> GraphDataset::split(const std::vector<float>& splits) {
    std::vector<GraphDataset> datasets;
    size_t start = 0;
    for (float split : splits) {
        size_t to = size().value() * split;
        std::vector<size_t> idx_dataset(to - start);
        std::copy(idx.begin() + start, idx.begin() + to, idx_dataset.begin());
        datasets.emplace_back(datasetInternal, device, idx_dataset);
        start = to;
    }
    return datasets;
}


SequenceExample Stack::apply_batch(std::vector<SequenceExample> examples) {
    std::vector<torch::Tensor> data, sizes, targets;
    data.reserve(examples.size());
    sizes.reserve(examples.size());
    targets.reserve(examples.size());
    for (auto &example : examples) {
        data.push_back(std::move(example.data));
        sizes.push_back(std::move(example.sizes));
        targets.push_back(std::move(example.labels));
    }
    return {torch::stack(data), torch::stack(sizes), torch::stack(targets)};
}
