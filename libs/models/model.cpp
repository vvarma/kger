//
// Created by nvr on 28/8/19.
//
#include "models/model.h"

void SequenceLabelDataset::collect(std::vector<int> tokens, key_type label) {

    this->tokens.push_back(tokens);
    labels.push_back(label);
    if (label_map.find(label) == label_map.end()) {
        label_map[label] = label_count++;
        label_count_map[label] = 1;
    } else {
        label_count_map[label]++;
    }

    if (tokens.size() > max_token_length)
        max_token_length = tokens.size();
}


