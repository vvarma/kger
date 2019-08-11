//
// Created by nvr on 4/8/19.
//
#include <torch/torch.h>
#include <iostream>

int main() {
    std::vector a = {1, 2, 3};
    a.resize(5,-1);
    auto c = torch::tensor(a);
    std::cout << c.view({5}) << std::endl;
    torch::Device device = torch::kCPU;
    if (torch::cuda::is_available()) {
        std::cout << "CUDA is available! Training on GPU." << std::endl;
        device = torch::kCUDA;
    }
}
