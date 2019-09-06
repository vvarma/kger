//
// Created by nvr on 4/8/19.
//

#include "EntityModeller.h"


EntityModellerImpl::EntityModellerImpl(int max_num_tokens, int label_count) :
        embedding_layer(register_module("embedding", torch::nn::Embedding(max_num_tokens, 128))),
        lstm(register_module("lstm", torch::nn::LSTM(torch::nn::LSTMOptions(128, 128).batch_first(true)))),
        inner_model(register_module("inner", torch::nn::Sequential(
                torch::nn::Linear(128, label_count),
                torch::nn::Functional(torch::log_softmax, 0, torch::kFloat32)
        ))) {
}

torch::Tensor EntityModellerImpl::forward(torch::Tensor token_seq_batch, torch::Tensor seq_sizes) {
    auto batch_size = token_seq_batch.size(0);
    auto x = embedding_layer(token_seq_batch);
    auto cell_state = torch::rand({1, batch_size, 128},
                                  torch::TensorOptions().requires_grad(false).device(token_seq_batch.device()));
    auto hidden_state = torch::rand({1, batch_size, 128},
                                    torch::TensorOptions().requires_grad(false).device(token_seq_batch.device()));
    auto state = torch::stack({cell_state, hidden_state});
    auto op = lstm(x, state);
    torch::Tensor last_state = op.state[0][-1];
    return inner_model->forward(last_state);
}

