//
// Created by nvr on 27/8/19.
//
#include <vector>
#include <map>

#ifndef KGER_MODEL_H
#define KGER_MODEL_H
typedef uint32_t key_type;

struct SequenceLabelDataset {
    int label_count;
    int max_token_length;
    std::vector<std::vector<int>> tokens;
    std::vector<std::vector<key_type>> labels;
    std::map<key_type, int> label_map;
    std::map<key_type, int> label_count_map;

    void collect(std::vector<int> tokens, std::vector<key_type> label);
};


#endif //KGER_MODEL_H
