//
// Created by nvr on 21/7/19.
//

#include "vocabulary/vocabulary.h"

Vocabulary::Vocabulary(const std::string &model_path) {
    auto status = processor.Load(model_path);
    ok = status.ok();
    numDocs = 0;
}

std::map<int, int> Vocabulary::process(const std::string &text) {
    std::vector<int> ids;
    processor.Encode(text, &ids);
    std::map<int, int> termFrequencies;
    for (int id:ids) {
        auto it = termFrequencies.find(id);
        if (it == termFrequencies.end()) {
            termFrequencies[id] = 1;
        } else {
            ++termFrequencies[id];
        }
    }
    for (auto p:termFrequencies) {
        auto it = documentFrequencies.find(p.first);
        if (it == documentFrequencies.end()) {
            documentFrequencies[p.first] = 1;
        } else {
            ++documentFrequencies[p.first];
        }
    }
    ++numDocs;
    return termFrequencies;
}

int Vocabulary::get_doc_count(int vocabId) const {
    auto it = documentFrequencies.find(vocabId);
    if (it == documentFrequencies.end()) {
        return 0;
    }
    return it->second;
}

std::vector<int> Vocabulary::tokenize(const std::string &text) {
    std::vector<int> ids;
    processor.Encode(text, &ids);
    return ids;
}

int Vocabulary::get_num_tokens() const {
    return processor.GetPieceSize();
}

int Vocabulary::get_padding_id() const {
    return processor.pad_id();
}
