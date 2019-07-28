//
// Created by nvr on 21/7/19.
//

#include <iostream>
#include "vocabulary.h"

Vocabulary::Vocabulary(const string &model_path) {
    auto status = processor.Load(model_path);
    ok = status.ok();
    numDocs = 0;
}

map<int, int> Vocabulary::process(const string &text) {
    vector<int> ids;
    processor.Encode(text, &ids);
    map<int, int> termFrequencies;
    for (int id:ids) {
        auto it = termFrequencies.find(id);
        if (it == termFrequencies.end()) {
            termFrequencies[id] = 1;
        } else {
            ++termFrequencies[id];
        }
    }
    for (pair<int, int> p:termFrequencies) {
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

string Vocabulary::get_token(int vocabId) const {
    return processor.DecodeIds({vocabId});
}

bool Vocabulary::is_stop(int vocabId, float threshold) const {
    return numDocs > 100000 && get_doc_count(vocabId) > numDocs * threshold;
}
