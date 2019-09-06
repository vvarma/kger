//
// Created by nvr on 28/8/19.
//
#include "models/model.h"

void SequenceLabelDataset::collect(std::vector<int> tokens, std::vector<key_type> label) {

    this->tokens.push_back(tokens);
    labels.push_back(label);
    for (auto l:label) {
        if (label_map.find(l) == label_map.end()) {
            label_map[l] = label_count++;
            label_count_map[l] = 1;
        } else {
            label_count_map[l]++;
        }

    }
    if (tokens.size() > max_token_length)
        max_token_length = tokens.size();
}


