#include <utility>

//
// Created by nvr on 3/8/19.
//

#ifndef KGER_GRAPHDATASET_H
#define KGER_GRAPHDATASET_H

#include <torch/torch.h>
#include "node.h"
#include "vocabulary.h"

struct SequenceExample {
    torch::Tensor data;
    torch::Tensor sizes;
    torch::Tensor labels;

    SequenceExample(torch::Tensor data, torch::Tensor sizes, torch::Tensor labels) : data(std::move(data)),
                                                                                     sizes(std::move(sizes)),
                                                                                     labels(std::move(labels)) {}
};
struct Stack :public torch::data::transforms::Collation<SequenceExample>{
    SequenceExample apply_batch(std::vector<SequenceExample> input_batch) override;
};

class GraphDataset : public torch::data::datasets::Dataset<GraphDataset, SequenceExample> {
    int label_count;
    int max_token_length;
    int padding_id;
    torch::Device device;
    std::vector<std::vector<int>> tokens;
    std::vector<has_unique_id::uid> labels;
    std::map<has_unique_id::uid, int> label_map;
public:
    explicit GraphDataset(std::shared_ptr<Graph> g, const std::shared_ptr<Vocabulary> &vocabulary,
                          torch::Device device);

    SequenceExample get(size_t index) override;

    c10::optional<size_t> size() const override;

    int get_label_count() const;
};

#endif //KGER_GRAPHDATASET_H
