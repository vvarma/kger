//
// Created by nvr on 21/7/19.
//

#ifndef KGER_VOCABULARY_H
#define KGER_VOCABULARY_H

#include <sentencepiece_processor.h>
#include <map>


class Vocabulary {
    sentencepiece::SentencePieceProcessor processor;
    std::map<int, int> documentFrequencies;
public:
    bool ok;
    int numDocs;

    Vocabulary(const std::string &model_path);

    std::map<int, int> process(const std::string &text);

    std::vector<int> tokenize(const std::string &text);

    int get_doc_count(int vocabId) const;

    int get_num_tokens() const;

    int get_padding_id() const;

};

#endif //KGER_VOCABULARY_H
