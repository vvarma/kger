//
// Created by nvr on 18/7/19.
//

#ifndef KGER_RELATION_PARSER_H
#define KGER_RELATION_PARSER_H


#include <string>
#include <functional>

using namespace std;

class relation_parser {
public:
    void parse(const string &filename, const function<void(vector<string_view>)>& collector);

};


#endif //KGER_RELATION_PARSER_H
