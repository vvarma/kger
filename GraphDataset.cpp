//
// Created by nvr on 3/8/19.
//

#include "GraphDataset.h"
#include "vocabulary.h"

SequenceExample GraphDataset::get(size_t index) {
    auto v = tokens[index];
    // not using padding id since its -1
    v.resize(max_token_length, 0);
    std::vector<int> nv(v.begin(), v.begin() + 80);
    auto t = torch::tensor(nv, torch::TensorOptions().dtype(torch::kInt64).requires_grad(false).device(device));
    auto l = torch::scalar_tensor(label_map[labels[index]], torch::TensorOptions().dtype(torch::kLong).requires_grad(
            false).device(device));
    auto s = torch::scalar_tensor(static_cast<long>(v.size()),
                                  torch::TensorOptions().dtype(torch::kLong).requires_grad(false).device(device));
    return SequenceExample(t, s, l);
}

c10::optional<size_t> GraphDataset::size() const {
    return tokens.size();
}

int GraphDataset::get_label_count() const {
    return label_count;
}

GraphDataset::GraphDataset(std::shared_ptr<Graph> g, const std::shared_ptr<Vocabulary> &vocabulary,
                           torch::Device device) : label_count(0),
                                                   max_token_length(
                                                           0),
                                                   padding_id(
                                                           vocabulary->get_padding_id()), device(device) {
    g->collect([&g, this](std::shared_ptr<InstanceNode> in, std::set<std::shared_ptr<NamedEntityNode>> nens,
                          std::set<std::shared_ptr<LexicalNode>> lns) -> void {
        for (const auto &ln:lns) {
            for (const auto &nen:nens) {
                this->tokens.push_back(ln->tokens);
                this->labels.push_back(nen->unique_id());
                auto neid = nen->unique_id();
                if (label_map.find(neid) == label_map.end()) {
                    label_map[neid] = label_count++;
                    label_count_map[neid] = 1;
                } else {
                    label_count_map[neid]++;
                }
                if (ln->tokens.size() > max_token_length)
                    max_token_length = ln->tokens.size();
            }
        }
    });

}

torch::Tensor GraphDataset::labelWeights() const {
    std::vector<float> w(label_count);
    for (auto p:label_count_map) {
        auto it = label_map.find(p.first);
        w[it->second] = static_cast<float>(p.second) / labels.size();
    }
    return torch::tensor(w, torch::TensorOptions().device(device).requires_grad(false));
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
