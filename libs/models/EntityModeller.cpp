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
//                torch::nn::Functional(static_cast<torch::Tensor(*)(const torch::Tensor &, int64_t)>(torch::log_softmax),
//                                      0)
        ))) {
}

torch::Tensor EntityModellerImpl::forward(torch::Tensor token_seq_batch, torch::Tensor seq_sizes) {
    auto batch_size = token_seq_batch.size(0);
//    std::cout << "input" << std::endl;
//    std::cout << token_seq_batch << std::endl;
//    std::cout << seq_sizes.sizes() << std::endl;
//    torch::_pack_padded_sequence(token_seq_batch, seq_sizes, true);
    auto x = embedding_layer(token_seq_batch);
//    std::cout << "embedding" << std::endl;
//    std::cout << x << std::endl;
//    std::cout << batch_size << " Size of batch" << std::endl;
    auto cell_state = torch::rand({1, batch_size, 128},
                                  torch::TensorOptions().requires_grad(false).device(token_seq_batch.device()));
    auto hidden_state = torch::rand({1, batch_size, 128},
                                    torch::TensorOptions().requires_grad(false).device(token_seq_batch.device()));
    auto state = torch::stack({cell_state, hidden_state});
//    std::cout << "state" << std::endl;
//    std::cout << state << std::endl;
    auto op = lstm(x, state);
//    std::cout << "lstm" << std::endl;
//    std::cout << op.output << std::endl;
//    auto unpacked = torch::unbind(op.state);
//    assert(unpacked.size()==2);
    torch::Tensor last_state = op.state[0][-1];
    return inner_model->forward(last_state);
}

//void EntityModellerImplV::to(torch::Device device, bool non_blocking) {
//    embedding_layer->to(device, non_blocking);
//    lstm->to(device,non_blocking);
//    inner_model->to(device, non_blocking);
//}
