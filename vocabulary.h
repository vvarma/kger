//
// Created by nvr on 21/7/19.
//

#ifndef KGER_VOCABULARY_H
#define KGER_VOCABULARY_H

#include <sentencepiece_processor.h>
#include <map>

using namespace std;

class Vocabulary {
    sentencepiece::SentencePieceProcessor processor;
    map<int, int> documentFrequencies;
public:
    bool ok;
    int numDocs;

    Vocabulary(const string &model_path);

    map<int, int> process(const string &text);

    int get_doc_count(int vocabId) const;

    string get_token(int vocabId) const;

    bool is_stop(int vocabId, float threshold) const;
};

#endif //KGER_VOCABULARY_H
