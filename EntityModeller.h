//
// Created by nvr on 4/8/19.
//

#ifndef KGER_ENTITYMODELLER_H
#define KGER_ENTITYMODELLER_H


#include <torch/torch.h>
#include "vocabulary.h"
#include <memory>

struct ModelBuilderOptions {
};

struct EntityModellerImpl : torch::nn::Module {
    torch::nn::Embedding embedding_layer;
    torch::nn::LSTM lstm;
    torch::nn::Sequential inner_model;

    explicit EntityModellerImpl(const std::shared_ptr<Vocabulary> &vocabulary, int label_count);

    torch::Tensor forward(torch::Tensor token_seq_batch, torch::Tensor seq_sizes);

};
TORCH_MODULE(EntityModeller);

#endif //KGER_ENTITYMODELLER_H
