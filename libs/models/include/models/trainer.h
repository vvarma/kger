//
// Created by nvr on 28/8/19.
//
#include "models/model.h"

#ifndef KGER_TRAINER_H
#define KGER_TRAINER_H

struct ModelOptions {
    int max_num_tokens;
};

void train(std::shared_ptr<SequenceLabelDataset>, const ModelOptions &);

#endif //KGER_TRAINER_H
