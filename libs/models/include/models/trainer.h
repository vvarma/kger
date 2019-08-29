//
// Created by nvr on 28/8/19.
//
#include "models/model.h"

#ifndef KGER_TRAINER_H
#define KGER_TRAINER_H

struct ModelOptions {
    int max_num_tokens;
    int num_epochs = 50;
    int batch_size = 1024;
    int num_dataloader_workers = 3;
    std::vector<float> splits = {0.8, 0.9, 1.0};
};

void train(std::shared_ptr<SequenceLabelDataset>, const ModelOptions &);

#endif //KGER_TRAINER_H
